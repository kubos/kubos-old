Telemetry+GraphQL Prototype
===========================

This is a pretty basic prototype of Graphene+SQLAlchemy+Flask
running against the ICEye telemetry database.

This app expects the database to be in a file named `telem.db`
in this folder and will attempt to use the table name `Telemetry`.

The default Telemetry table is pretty large and there are no query limits
built into the code atm. I'd suggest trimming down the table size in the
database before playing.


::

   python ./app.py

This will start a flask server at http://127.0.0.1:5000.
If you go to http://127.0.0.1:5000/graphql you will find a built-in
GraphiQL interface.

Valid queries will take the form of

::

   {
     allTelemetry {
       edges {
         node {
           timestamp,
           subsystem,
         }
       }
     }
   }
