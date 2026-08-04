from json import dumps, loads

from aiokafka import AIOKafkaConsumer, AIOKafkaProducer
from core.constants import kafka_topic
from pydantic import BaseModel
from schemas.event import AddRenderProjectEvent
from services.project import ProjectService

from infrastructure.database.core import session_factory
from infrastructure.database.unit_of_work import UnitOfWork


def serialize_message[T: BaseModel](message_data: T) -> bytes:
    message_dict = message_data.model_dump()
    serialized_value = dumps(message_dict)
    encoded_serialized_value = serialized_value.encode()
    return encoded_serialized_value


def deserialize_message(message: bytes) -> dict[str, str | int | bool]:
    message_string = message.decode()
    json_message = loads(message_string)
    return json_message  # type: ignore[no-any-return]


async def add_render_to_project_consume(consumer: AIOKafkaConsumer) -> None:
    await consumer.start()
    async for message in consumer:
        json_data = message.value
        add_render_project_event = AddRenderProjectEvent.model_validate(
            json_data,
        )
        project_id = add_render_project_event.project_id
        async with (
            session_factory() as session,
            UnitOfWork(session) as unit_of_work,
            ProjectService(unit_of_work) as project_service,
        ):
            await project_service.update_project_status(project_id)
            await project_service.add_render_to_project(
                add_render_project_event,
            )

        await consumer.commit()


add_render_to_project_consumer = AIOKafkaConsumer(
    kafka_topic.generate_model,
    bootstrap_servers="kafka:9092",
    group_id="app",
    value_deserializer=deserialize_message,
)


generate_render_producer = AIOKafkaProducer(
    bootstrap_servers="kafka:9092",
    value_serializer=serialize_message,
)
