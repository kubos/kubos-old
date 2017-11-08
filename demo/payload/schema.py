import graphene

class PayloadTelemetry(graphene.ObjectType):
    """Class modeling the payload telemetry"""

    # Member modeling payload on/off state
    payload_on = graphene.Boolean()
    uptime = graphene.Int()
    processor_temp = graphene.Float()

# Local payload instance so we can have persistence
my_payload = PayloadTelemetry(payload_on=False,
                            uptime=0,
                            processor_temp=0)

class ThrusterTelemetry(graphene.ObjectType):
    """Class modeling the thruster telemetry"""

    # Member modeling payload on/off state
    thruster_on = graphene.Boolean()
    fuel_temp = graphene.Float()
    nozzle_temp = graphene.Float()

# Local thruster instance so we can have persistence
my_thruster = ThrusterTelemetry(thruster_on=False,
                                fuel_temp=0,
                                nozzle_temp=0)

class Query(graphene.ObjectType):
    """Query class used to define GraphQL query structures
    """
    payload = graphene.Field(PayloadTelemetry)
    thruster = graphene.Field(ThrusterTelemetry)

    def resolve_payload(self, info):
        """gets payload telemetry"""
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
        my_payload.payload_on = payload_on
        return my_payload

class Mutation(graphene.ObjectType):
    payload_enable = PayloadEnable.Field()

schema = graphene.Schema(query=Query, mutation=Mutation)
