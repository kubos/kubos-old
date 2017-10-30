"""
This module creates the schema bindings between GraphQL queries and
SQLAlchemy models. In this case there is a single query and single
binding dealing with the Telemetry table.
"""
import graphene
from graphene_sqlalchemy import SQLAlchemyObjectType
from models import Telemetry as TelemetryModel


class Telemetry(SQLAlchemyObjectType):
    """Telemetry class for use in Graphene queries
    """
    class Meta:
        """Meta class for pointing to TelemetryModel class
        """
        model = TelemetryModel


class Query(graphene.ObjectType):
    """Query class used to define GraphQL query structures
    """
    telemetry = graphene.List(Telemetry,
                              first=graphene.Int(),
                              subsystem=graphene.String(),
                              param=graphene.String(),
                              timestamp_before=graphene.Float(),
                              timestamp_after=graphene.Float())

    def resolve_telemetry(self,
                          info,
                          first=None,
                          subsystem=None,
                          param=None,
                          timestamp_before=None,
                          timestamp_after=None):
        model = Telemetry._meta.model
        query = Telemetry.get_query(info)
        print(type(query))
        if subsystem:
            query = query.filter(model.subsystem == subsystem)
        if param:
            query = query.filter(model.param == param)
        if timestamp_before:
            query = query.filter(model.timestamp <= timestamp_before)
        if timestamp_after:
            query = query.filter(model.timestamp >= timestamp_after)
        if first:
            return query.limit(first)
        return query.all()

schema = graphene.Schema(query=Query)
