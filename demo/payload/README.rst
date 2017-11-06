Python Payload Demo
====================

This is a barebones demo of a payload implemented in Python.

This payload listens on http://127.0.0.1:5002 for
graphql mutations and queries. Mutations represent commands,
queries are requests for state information (telemetry).

There is also a graphiql interface at http://127.0.0.1:5002/graphiql
for ease of development.

Currently this payload has a single member and function `on`.

Example query:

.. code::
   {
       payload {
           on
       }
   }

Example function call (mutation):

.. code::
   mutation {
       enable(on:true) {
           on
       }
   }
