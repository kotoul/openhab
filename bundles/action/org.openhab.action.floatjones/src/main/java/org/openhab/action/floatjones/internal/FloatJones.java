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
	static float prevDowJones;
	
	@ActionDoc(text="A cool method that does some FloatJones")
	public static void setFloatJones(
			@ParamDoc(name="new value") float dowJones) {
		
		if (prevDowJones != FloatJones.dowJones) {
			logger.debug("prev float jones set from " + prevDowJones + " to " + FloatJones.dowJones);
			prevDowJones = FloatJones.dowJones;
		}
		FloatJones.dowJones = dowJones;
		logger.debug("new float jones is " + FloatJones.dowJones);
	}
	
	@ActionDoc(text="A cool method that does some FloatJones", returns="-1 -> decreased, 1 -> increased")
	public static int valueIncreased() {
		
		boolean increased = prevDowJones < dowJones;
		logger.debug("value increased: " + increased);
		return increased ? 1 : -1;
	}
	
	@ActionDoc(text="A cool method that does some FloatJones")
	public static String calcChange() {
		
		String result = String.format("%.2f", dowJones - prevDowJones);
		result += String.format(" (%.2f%%)", (dowJones - prevDowJones) * 100 / prevDowJones);
		logger.debug("calcChange returning " + result);
		return result;
	}
}
