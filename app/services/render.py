from aiokafka import AIOKafkaProducer
from core.constants import kafka_topic
from core.interfaces.services import AbstractRenderService
from schemas.render import RenderCreate


class RenderService(AbstractRenderService):
    def __init__(self, producer: AIOKafkaProducer) -> None:
        self.producer = producer

    async def send_event_render_model(self, create_render_data: RenderCreate) -> None:
        await self.producer.send(
            topic=kafka_topic.create_project,
            value=create_render_data,
        )
