Python Radio Demo
===========================

This is a barebones demo of the radio functionality implemented in Python. For
the purposes of this demo we are basically skipping the "radio" functionality
and focusing on the communication aspects.

This program will serve two main purposes:
 - Routing messages from ground control to subsystems
 - Routing messages from subsytems to ground control


Currently the process listens on http://127.0.0.1:5000. It presents two endpoints:
 - http://127.0.0.1:5000/majortom
   - This endpoint is for receiving messages from major tom
   - All messages sent to this endpoint will be examined and sent to the correct subsystem
 - http://127.0.0.1:5000/kubos
   - This endpoint is for receiving messages from subsystems
   - All messages sent to this endpoint will be routed to major tom

These assumptions are made about where major tom and subsystems will live:
 - Radio - http://127.0.0.1:5000
 - Telemetry - http://127.0.0.1:5001
 - Payload - http://127.0.0.1:5002
 - MajorTom - http://127.0.0.1:5003
