import json

from core.interfaces.kafka import KafkaProducer
from core.interfaces.services import AbstractRenderService
from schemas.render import RenderCreate


class RenderService(AbstractRenderService):
    def __init__(self, producer: KafkaProducer) -> None:
        self.producer = producer
        self.topic = "create_render"

    async def create_render(self, create_render_data: RenderCreate) -> None:
        render_data = create_render_data.model_dump()
        serialized_value = json.dumps(render_data)
        self.producer.produce(topic=self.topic, value=serialized_value)  # type: ignore[call-arg]
        await self.producer.flush()
