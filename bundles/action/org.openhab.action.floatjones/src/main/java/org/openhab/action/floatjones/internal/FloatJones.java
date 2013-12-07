/**
 * Copyright (c) 2010-2013, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.action.floatjones.internal;

import org.openhab.core.scriptengine.action.ActionDoc;
import org.openhab.core.scriptengine.action.ParamDoc;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


/**
 * This class contains the methods that are made available in scripts and rules for FloatJones.
 * 
 * @author tomk
 * @since 1.4.0
 */
public class FloatJones {

	private static final Logger logger = LoggerFactory.getLogger(FloatJones.class);

	static float dowJones;
	
	@ActionDoc(text="A cool method that does some FloatJones")
	public static void setFloatJones(
			@ParamDoc(name="Dow Jones as Float") float dowJones) {
		
		FloatJones.dowJones = dowJones;
	}
	
	@ActionDoc(text="A cool method that does some FloatJones")
	public static float getFloatJones() {
		
		return dowJones;
	}
	
	@ActionDoc(text="A cool method that does some FloatJones")
	public static String calcChange(
			@ParamDoc(name="Last value") float prev, 
			@ParamDoc(name="New value") float now) {
		
		String result = String.format("%.2f", now - prev);
		result += String.format(" (%.2f%%)", (now - prev) * 100 / prev);
		logger.debug("returning " + result);
		return result;
	}
}
