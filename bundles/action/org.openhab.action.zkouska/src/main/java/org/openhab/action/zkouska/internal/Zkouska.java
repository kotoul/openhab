/**
 * Copyright (c) 2010-2013, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.action.zkouska.internal;

import org.openhab.core.scriptengine.action.ActionDoc;
import org.openhab.core.scriptengine.action.ParamDoc;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


/**
 * This class contains the methods that are made available in scripts and rules for Zkouska.
 * 
 * @author tomk
 * @since 1.4.0
 */
public class Zkouska {

	private static final Logger logger = LoggerFactory.getLogger(Zkouska.class);

	// provide public static methods here
	
	// Example
	@ActionDoc(text="A cool method that does some Zkouska")
	public static boolean doZkouska(
			@ParamDoc(name="something") String something) {
		
		if (!ZkouskaActionService.isProperlyConfigured) {
			logger.debug("Zkouska action is not yet configured - execution aborted!");
			return false;
		}
		// now do something cool
		logger.debug("Zkouska action IS properly configured - execution started!");
		logger.info(something);
		return true;
	}
	
	@ActionDoc(text="A cool method that does some Zkouska")
	public static float doConvert(
			@ParamDoc(name="Dow Jones as String") String dowJonesString) {
		
		float dowJones = 0;
		
		if (dowJonesString != null) {
			if (dowJonesString.contains(",")) {
				dowJonesString = dowJonesString.replace(",","");
			}
			try {
				dowJones = Float.parseFloat(dowJonesString);
				logger.debug(dowJonesString + " successfuly converted to float: " + dowJones);
			} catch (NumberFormatException e) {
				logger.debug("Cannot convert " + dowJonesString + " to float");
			}
		}
		
		return dowJones;
	}

}
