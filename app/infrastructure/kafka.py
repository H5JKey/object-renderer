from json import dumps, loads

from aiokafka import AIOKafkaConsumer, AIOKafkaProducer
from core.constants import kafka_topic
from pydantic import BaseModel
from schemas.file import FileCreate
from schemas.render import UploadRenderProjectEvent
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
    message_json = loads(message_string)
    return message_json  # type: ignore[no-any-return]


async def consume_messages(consumer: AIOKafkaConsumer) -> None:
    await consumer.start()
    async for message in consumer:
        json_data = message.value
        project_id = json_data["project_id"]
        json_data.pop("project_id")
        file = FileCreate(**json_data["file"])
        uploaded_render_project_event = UploadRenderProjectEvent(
            project_id=project_id,
            file=file,
        )
        async with (
            session_factory() as session,
            UnitOfWork(session) as unit_of_work,
            ProjectService(unit_of_work) as project_service,
        ):
            await project_service.update_project_status(project_id)
            await project_service.upload_render_to_project(
                uploaded_render_project_event,
            )
        await consumer.commit()


consumer = AIOKafkaConsumer(
    kafka_topic.generate_model,
    bootstrap_servers="kafka:9092",
    group_id="app",
    value_deserializer=deserialize_message,
)


producer = AIOKafkaProducer(
    bootstrap_servers="kafka:9092",
    value_serializer=serialize_message,
)
