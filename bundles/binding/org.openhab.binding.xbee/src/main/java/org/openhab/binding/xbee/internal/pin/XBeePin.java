/**
 * Copyright (c) 2010-2014, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.binding.xbee.internal.pin;

/**
 * @author Antoine Bertin
 * @since 1.3.0
 */
public class XBeePin {
	public XBeePinType pinType;
	public int pinNumber;

	public XBeePin(String pin) {
		if (pin.startsWith("A")) {
			pinType = XBeePinType.ANALOG;
		} else if (pin.startsWith("D")) {
			pinType = XBeePinType.DIGITAL;
		} else {
			throw new IllegalArgumentException("Pin should start with A (for analog) or D (for digital)");
		}
		pinNumber = Integer.parseInt(pin.substring(1));
	}

	@Override
	public String toString() {
		String repr;
		if (pinType == XBeePinType.ANALOG) {
			repr = "A";
		} else {
			repr = "B";
		}
		repr += Integer.toString(pinNumber);
		return repr;
	}
}
