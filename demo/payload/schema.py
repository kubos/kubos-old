import graphene,time

class PayloadTelemetry(graphene.ObjectType):
    """Class modeling the payload telemetry
    This class could also contain functions for manipulating
    the payload or retrieving hardware-based state values
    from the actual payload.
    """

    # Member modeling payload on/off state
    payload_on = graphene.Boolean()
    uptime = graphene.Float()
    start_time = graphene.Float()

# Local payload instance so we can have persistence
my_payload = PayloadTelemetry(payload_on=False,
                            uptime=0,
                            start_time=0)

class ThrusterTelemetry(graphene.ObjectType):
    """Class modeling the thruster telemetry.
    This class could also contain functions for manipulating
    the thruster or retrieving hardware-based state values
    from the actual thruster."""

    # Member modeling payload on/off state
    thruster_on = graphene.Boolean()

# Local thruster instance so we can have persistence
my_thruster = ThrusterTelemetry(thruster_on=False)

class Query(graphene.ObjectType):
    """Query class used to define GraphQL query structures
    """
    payload = graphene.Field(PayloadTelemetry)
    thruster = graphene.Field(ThrusterTelemetry)

    def resolve_payload(self, info):
        """This function is called when a query for the Payload
        is received. Any function calls to retrieve or refresh live
        payload data would be called here."""
        if my_payload.payload_on == True:
            my_payload.uptime = time.time() - my_payload.start_time
        else:
            my_payload.uptime = 0

        return my_payload
    def resolve_thruster(self, info):
        """This function is called when a query for Thruster
        data is received. Any function calls to retrieve live
        thruster data would be called here to ensure
        updated information."""
        return my_thruster

class PayloadEnable(graphene.Mutation):
    """Mutation class for modeling enable mutation for Payload"""
    class Arguments:
        payload_on = graphene.Boolean(required=True)

    Output = PayloadTelemetry

    def mutate(self, info, payload_on):
        """This function is called when a mutation of type payloadOn
        is received.
        Any user logic for enabling the payload such as I2C/SPI calls
        or custom logic should be called here."""
        if (my_payload.payload_on == False and payload_on == True):
            my_payload.start_time = time.time()
        elif (my_payload.payload_on == True and payload_on == False):
            my_payload.start_time = 0
        my_payload.payload_on = payload_on
        return my_payload

class ThrusterEnable(graphene.Mutation):
    """Mutation class for modeling enable mutation for Payload"""
    class Arguments:
        thruster_on = graphene.Boolean(required=True)

    Output = ThrusterTelemetry

    def mutate(self, info, thruster_on):
        """This function is called when a mutation of type thrusterOn
        is received.
        Any user logic for enabling the payload thruster such as I2C/SPI calls
        or custom logic should get called here."""
        my_thruster.thruster_on = thruster_on
        return my_thruster

class Mutation(graphene.ObjectType):
    payload_enable = PayloadEnable.Field()
    thruster_enable = ThrusterEnable.Field()

schema = graphene.Schema(query=Query, mutation=Mutation)
