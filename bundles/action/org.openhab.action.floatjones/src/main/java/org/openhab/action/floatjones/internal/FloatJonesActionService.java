/**
 * Copyright (c) 2010-2013, openHAB.org and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 */
package org.openhab.action.floatjones.internal;

import java.util.Dictionary;

import org.openhab.core.scriptengine.action.ActionService;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
	

/**
 * This class registers an OSGi service for the FloatJones action.
 * 
 * @author tomk
 * @since 1.4.0
 */
public class FloatJonesActionService implements ActionService, ManagedService {

	private static final Logger logger = LoggerFactory.getLogger(FloatJonesActionService.class);

	/**
	 * Indicates whether this action is properly configured which means all
	 * necessary configurations are set. This flag can be checked by the
	 * action methods before executing code.
	 */
	/* default */ static boolean isProperlyConfigured = false;
	
	public FloatJonesActionService() {
	}
	
	public void activate() {
	}
	
	public void deactivate() {
		// deallocate Resources here that are no longer needed and 
		// should be reset when activating this binding again
	}

	@Override
	public String getActionClassName() {
		return FloatJones.class.getCanonicalName();
	}

	@Override
	public Class<?> getActionClass() {
		return FloatJones.class;
	}

	/**
	 * @{inheritDoc}
	 */
	@Override
	public void updated(Dictionary<String, ?> config) throws ConfigurationException {
		if (config != null) {
			logger.debug("Config found!");
			// read config parameters here ...

			isProperlyConfigured = true;
		}
		isProperlyConfigured = true;
	}
	
}
