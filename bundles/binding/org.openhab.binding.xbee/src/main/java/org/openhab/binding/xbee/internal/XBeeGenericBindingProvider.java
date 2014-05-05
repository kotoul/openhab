/**
 * Copyright (c) 2010-2014, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.binding.xbee.internal;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.openhab.binding.xbee.XBeeBindingProvider;
import org.openhab.binding.xbee.internal.pin.XBeePin;
import org.openhab.core.binding.BindingConfig;
import org.openhab.core.items.Item;
import org.openhab.core.library.types.StringType;
import org.openhab.core.types.Command;
import org.openhab.core.types.TypeParser;
import org.openhab.model.item.binding.AbstractGenericBindingProvider;
import org.openhab.model.item.binding.BindingConfigParseException;
import org.openhab.model.item.binding.BindingConfigReader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.rapplogic.xbee.api.XBeeAddress;
import com.rapplogic.xbee.api.XBeeAddress16;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeRequest;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.wpan.RxResponse16;
import com.rapplogic.xbee.api.wpan.TxRequest16;
import com.rapplogic.xbee.api.zigbee.ZNetRxIoSampleResponse;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;

/**
 * This class is responsible for parsing the binding configuration.
 * 
 * @author Antoine Bertin
 * @since 1.3.0
 */
public class XBeeGenericBindingProvider extends AbstractGenericBindingProvider implements XBeeBindingProvider,
		BindingConfigReader {

	/**
	 * Artificial command for the in-binding configuration (which has no command
	 * part by definition). Because we use this artificial command we can reuse
	 * the {@link XBeeBindingConfig} for both in- and out-configuration.
	 */
	protected static final Command IN_BINDING_KEY = StringType.valueOf("IN_BINDING");

	/** {@link Pattern} which matches for an IoSampleResponse In-Binding */
	private static final Pattern IOSAMPLERESPONSE_IN_PATTERN = Pattern
			.compile("<(?<responseType>\\w+)@(?<address>([0-9a-zA-Z])+)#(?<pin>[AD][0-9]{1,2})(:(?<transformation>.*))?");

	/** {@link Pattern} which matches for an RxResponse In-Binding */
	private static final Pattern RXRESPONSE_IN_PATTERN = Pattern
			.compile("<\\[(?<responseType>\\w+)@(?<address>([0-9a-zA-Z])+)#(?<dataOffset>\\d+)(\\[(?<dataType>\\w+)\\])(?<destItem>\\w+)?(:(?<firstByte>\\d{1,3}))?\\]");

	/** {@link Pattern} which matches an Out-Binding */
	private static final Pattern OUT_PATTERN = Pattern
			.compile(">\\[(?<command>.*?):(?<requestType>\\w+)@(?<address>([0-9a-zA-Z])+)#(?<payload>.*?)\\]");

	private static final Logger logger = LoggerFactory.getLogger(XBeeGenericBindingProvider.class);

	@Override
	public String getBindingType() {
		return "xbee";
	}

	@Override
	public void validateItemType(Item item, String bindingConfig) throws BindingConfigParseException {
		// Accept all sort of items
	}

	@Override
	public void processBindingConfiguration(String context, Item item, String bindingConfig)
			throws BindingConfigParseException {
		super.processBindingConfiguration(context, item, bindingConfig);

		// Create the config
		XBeeBindingConfig config = new XBeeBindingConfig();
		config.itemType = item.getClass();

		// Match patterns
		if (IOSAMPLERESPONSE_IN_PATTERN.matcher(bindingConfig).matches()) {
			Matcher matcher = IOSAMPLERESPONSE_IN_PATTERN.matcher(bindingConfig);
			while (matcher.find()) {
				XBeeInBindingConfigElement configElement;
				configElement = new XBeeInBindingConfigElement();

				// Parse the responseType
				if (matcher.group("responseType").equals("znetrxiosampleresponse")) {
					configElement.responseType = ZNetRxIoSampleResponse.class;
				} else {
					throw new BindingConfigParseException("Invalid binding configuration: responseType '"
							+ matcher.group("responseType") + "' is not a valid responseType");
				}

				// Parse the address, pin and transformation
				configElement.address = parseAddress(matcher.group("address"));
				configElement.pin = new XBeePin(matcher.group("pin"));
				configElement.transformation = matcher.group("transformation");

				// Add to the config
				logger.debug("Adding in-binding configElement: {}", configElement.toString());
				config.put(IN_BINDING_KEY, configElement);
			}
		}
		if (RXRESPONSE_IN_PATTERN.matcher(bindingConfig).matches()) {
			Matcher matcher = RXRESPONSE_IN_PATTERN.matcher(bindingConfig);
			while (matcher.find()) {
				XBeeInBindingConfigElement configElement;
				configElement = new XBeeInBindingConfigElement();

				// Parse the responseType
				if (matcher.group("responseType").equals("znetrxresponse")) {
					configElement.responseType = ZNetRxResponse.class;
				} else if (matcher.group("responseType").equals("rxresponse16")) {
					configElement.responseType = RxResponse16.class;
				} else {
					throw new BindingConfigParseException("Invalid binding configuration: responseType '"
							+ matcher.group("responseType") + "' is not a valid responseType");
				}

				// Parse the address, dataOffset, dataType and firstByte
				configElement.address = parseAddress(matcher.group("address"));
				configElement.dataOffset = Integer.parseInt(matcher.group("dataOffset"));
				configElement.dataType = parseDataType(matcher.group("dataType"));
				configElement.destItem = matcher.group("destItem");
				configElement.firstByte = matcher.group("firstByte") != null ? Byte.parseByte(matcher
						.group("firstByte")) : null;

				// Add to the config
				logger.debug("Adding in-binding configElement: {}", configElement.toString());
				config.put(IN_BINDING_KEY, configElement);
			}
		}
		if (OUT_PATTERN.matcher(bindingConfig).matches()) {
			Matcher matcher = OUT_PATTERN.matcher(bindingConfig);
			while (matcher.find()) {
				XBeeOutBindingConfigElement configElement = new XBeeOutBindingConfigElement();

				Command command = createCommandFromString(item, matcher.group("command"));

				// Parse the responseType
				if (matcher.group("requestType").equals("txrequest16")) {
					configElement.requestType = TxRequest16.class;
				} else {
					throw new BindingConfigParseException("Invalid binding configuration: requestType '"
							+ matcher.group("requestType") + "' is not a valid requestType");
				}

				// Parse the address and payload
				configElement.address = parseAddress(matcher.group("address"));
				configElement.payload = parsePayload(matcher.group("payload"));

				// Add to the config
				logger.debug("Adding out-binding configElement: {}", configElement.toString());
				config.put(command, configElement);
			}
		}
		if (!IOSAMPLERESPONSE_IN_PATTERN.matcher(bindingConfig).matches()
				&& !RXRESPONSE_IN_PATTERN.matcher(bindingConfig).matches()
				&& !OUT_PATTERN.matcher(bindingConfig).matches()) {
			throw new BindingConfigParseException("Invalid binding configuration '" + bindingConfig + "'");
		}
		logger.debug("Adding binding config for item {}", item.getName());
		addBindingConfig(item, config);
	}

	private Command createCommandFromString(Item item, String commandAsString) throws BindingConfigParseException {
		Command command = TypeParser.parseCommand(item.getAcceptedCommandTypes(), commandAsString);

		if (command == null) {
			throw new BindingConfigParseException("couldn't create Command from '" + commandAsString + "' ");
		}

		return command;
	}

	@Override
	public Class<? extends Item> getItemType(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null ? config.itemType : null;
	}

	@Override
	public Class<? extends XBeeResponse> getResponseType(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).responseType : null;
	}

	@Override
	public XBeeAddress getAddress(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).address : null;
	}

	@Override
	public Integer getDataOffset(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).dataOffset : null;
	}

	@Override
	public Class<? extends Number> getDataType(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).dataType : null;
	}

	@Override
	public XBeePin getPin(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).pin : null;
	}

	@Override
	public String getTransformation(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).transformation : null;
	}

	@Override
	public String getDestItem(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).destItem : null;
	}

	@Override
	public Byte getFirstByte(String itemName) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(IN_BINDING_KEY) != null ? ((XBeeInBindingConfigElement) config
				.get(IN_BINDING_KEY)).firstByte : null;
	}

	@Override
	public Class<? extends XBeeRequest> getRequestType(String itemName, Command command) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(command) != null ? ((XBeeOutBindingConfigElement) config.get(command)).requestType
				: null;
	}

	@Override
	public XBeeAddress getAddress(String itemName, Command command) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(command) != null ? ((XBeeOutBindingConfigElement) config.get(command)).address
				: null;
	}

	@Override
	public int[] getPayload(String itemName, Command command) {
		XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
		return config != null && config.get(command) != null ? ((XBeeOutBindingConfigElement) config.get(command)).payload
				: null;
	}

	@Override
	public List<String> getInBindingItemNames() {
		List<String> inBindings = new ArrayList<String>();
		for (String itemName : bindingConfigs.keySet()) {
			XBeeBindingConfig config = (XBeeBindingConfig) bindingConfigs.get(itemName);
			if (config.containsKey(IN_BINDING_KEY)) {
				inBindings.add(itemName);
			}
		}
		return inBindings;
	}

	private XBeeAddress parseAddress(String address) throws BindingConfigParseException {
		XBeeAddress xbeeAddress;
		String[] addressString = address.split("(?<=\\G[0-9a-fA-F]{2})");
		int[] addressHex = new int[addressString.length];
		for (int i = 0; i < addressString.length; i++) {
			addressHex[i] = Integer.parseInt(addressString[i], 16);
		}
		switch (addressHex.length) {
		case 8:
			xbeeAddress = new XBeeAddress64(addressHex);
			break;
		case 2:
			xbeeAddress = new XBeeAddress16(addressHex);
			break;
		default:
			throw new BindingConfigParseException("Invalid binding configuration: address '" + address
					+ "' is not a valid address");
		}
		return xbeeAddress;
	}

	private Class<? extends Number> parseDataType(String dataType) {
		if (dataType == null) {
			return null;
		} else if (dataType.equals("float")) {
			return float.class;
		} else if (dataType.equals("int")) {
			return int.class;
		} else if (dataType.equals("byte")) {
			return byte.class;
		}
		return null;

	}

	private int[] parsePayload(String data) {
		int[] payload = new int[data.length()];
		for (int i = 0; i < payload.length; i++) {
			payload[i] = data.charAt(i);
		}
		// logger.debug("parsing payload [dataString={}, payload={}]", data,
		// payload);
		return payload;
	}

	static class XBeeBindingConfig extends HashMap<Command, BindingConfig> implements BindingConfig {
		/**
		 * Generated serial version uid
		 */
		private static final long serialVersionUID = 2541964231552108432L;
		Class<? extends Item> itemType;
	}

	static class XBeeInBindingConfigElement implements BindingConfig {
		Class<? extends XBeeResponse> responseType;
		XBeeAddress address;
		Integer dataOffset;
		Class<? extends Number> dataType;
		XBeePin pin;
		String transformation;
		String destItem;
		Byte firstByte;

		@Override
		public String toString() {
			String repr = responseType.getName() + "(";
			repr += address != null ? "address=" + address : "address=null";
			repr += dataOffset != null ? ", dataOffset=" + dataOffset : "";
			repr += dataType != null ? ", dataType=" + dataType : "";
			repr += pin != null ? ", pin=" + pin : "";
			repr += transformation != null ? ", transformation=" + transformation : "";
			repr += destItem != null ? ", destItem=" + destItem : "";
			repr += firstByte != null ? ", firstByte=" + firstByte : "";
			repr += ")";
			return repr;
		}
	}

	static class XBeeOutBindingConfigElement implements BindingConfig {
		Class<? extends XBeeRequest> requestType;
		XBeeAddress address;
		int[] payload;

		@Override
		public String toString() {
			String repr = requestType.getName() + "(";
			repr += address != null ? "address=" + address : "address=null";
			repr += payload != null ? ", payload=" + payload : "payload=null";
			repr += ")";
			return repr;
		}
	}

}
