#!/usr/bin/env python

# Copyright 2017 Kubos Corporation
# Licensed under the Apache License, Version 2.0
# See LICENSE file for details.

"""
Event publishing abstractions
"""

import dbus
import dbus.service
import dbus.glib
import json

# These name/path variables should probably be set
# via a config file or init function
obj_name = "kubos.handlers.payload"
obj_path = "/kubos/handlers/payload"

session_bus = None
bus_name = None
publisher = None

class Publisher(dbus.service.Object):

    def __init__(self, bus_name, object_path):
        dbus.service.Object.__init__(self, bus_name, object_path)

    @dbus.service.signal(obj_name)
    def publish(self, data):
        print "Published"
        print data


def publish_json(data):
    """
    Generic event publishing interface. Takes a raw json structure
    and publishes out the string equivalent.
    """
    global session_bus
    global bus_name
    global publisher

    if session_bus == None:
        session_bus = dbus.SessionBus()

    if bus_name == None:
        bus_name = dbus.service.BusName(obj_name, bus=session_bus)

    if publisher == None:
        publisher = Publisher(bus_name, obj_path)

    publisher.publish(json.dumps(data))
