import graphene,time

class PayloadTelemetry(graphene.ObjectType):
    """Class modeling the payload telemetry"""

    # Member modeling payload on/off state
    payload_on = graphene.Boolean()
    uptime = graphene.Float()
    start_time = graphene.Float()

# Local payload instance so we can have persistence
my_payload = PayloadTelemetry(payload_on=False,
                            uptime=0,
                            start_time=0)

class ThrusterTelemetry(graphene.ObjectType):
    """Class modeling the thruster telemetry"""

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
        """gets payload telemetry"""
        if my_payload.payload_on == True:
            my_payload.uptime = time.time() - my_payload.start_time
        else:
            my_payload.uptime = 0

        return my_payload
    def resolve_thruster(self, info):
        """gets thruster telemetry"""
        return my_thruster

class PayloadEnable(graphene.Mutation):
    """Mutation class for modeling enable mutation for Payload"""
    class Arguments:
        payload_on = graphene.Boolean(required=True)

    Output = PayloadTelemetry

    def mutate(self, info, payload_on):
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
        my_thruster.thruster_on = thruster_on
        return my_thruster

class Mutation(graphene.ObjectType):
    payload_enable = PayloadEnable.Field()
    thruster_enable = ThrusterEnable.Field()

schema = graphene.Schema(query=Query, mutation=Mutation)
