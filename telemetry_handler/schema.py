import graphene
from graphene import relay
from graphene_sqlalchemy import SQLAlchemyObjectType, SQLAlchemyConnectionField
from models import db_session, Telemetry as TelemetryModel

class Telemetry(SQLAlchemyObjectType):
    class Meta:
        model = TelemetryModel

class Query(graphene.ObjectType):
    telemetry = graphene.List(Telemetry,
                              subsystem=graphene.String(),
                              param=graphene.String(),
                              timestamp_before=graphene.Float(),
                              timestamp_after=graphene.Float())

    def resolve_telemetry(self,
                          info,
                          subsystem=None,
                          param=None,
                          timestamp_before=None,
                          timestamp_after=None):
        model = Telemetry._meta.model
        query = Telemetry.get_query(info)
        if subsystem:
            query = query.filter(model.subsystem == subsystem)
        if param:
            query = query.filter(model.param == param)
        if timestamp_before:
            query = query.filter(model.timestamp <= timestamp_before)
        if timestamp_after:
            query = query.filter(model.timestamp >= timestamp_after)
        return query.all()

schema = graphene.Schema(query=Query)
