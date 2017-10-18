import graphene
from graphene import relay
from graphene_sqlalchemy import SQLAlchemyObjectType, SQLAlchemyConnectionField
from models import db_session, Telemetry as TelemetryModel
""""
class Telemetry(SQLAlchemyObjectType):
    class Meta:
        model = TelemetryModel
        interfaces = (relay.Node, )

class TelemConnectionField(SQLAlchemyConnectionField):
    RELAY_ARGS = ['first', 'last', 'before', 'after']

    @classmethod
    def get_query(cls, model, context, info, args):
        query = super(TelemConnectionField, cls).get_query(model, context, info, args)
        for field, value in args.items():
            if field not in cls.RELAY_ARGS:
                query = query.filter(getattr(model, field) == value)
        return query

class Query(graphene.ObjectType):
    telemetery = TelemConnectionField(Telemetry,
                                           param=graphene.String(),
                                           subsystem=graphene.String(),
                                           timestamp=graphene.Int(),
                                           value=graphene.Int())


schema = graphene.Schema(query=Query)
"""

class Telemetry(SQLAlchemyObjectType):
    class Meta:
        model = TelemetryModel

class Query(graphene.ObjectType):
    telemetry = graphene.List(Telemetry)

    def resolve_telemetry(self, info, subsystem=None):
        print("resolving telemetry")
        return Telemetry.get_query(info).all()

schema = graphene.Schema(query=Query)
