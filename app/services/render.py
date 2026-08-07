from aiokafka import AIOKafkaProducer
from core.constants import kafka_topic
from core.interfaces.services import AbstractRenderService
from core.logging import get_logger
from schemas.render import RenderCreate

logger = get_logger(__name__)


class RenderService(AbstractRenderService):
    def __init__(self, producer: AIOKafkaProducer) -> None:
        self.producer = producer

    async def send_render_model_event(self, create_render_data: RenderCreate) -> None:
        await self.producer.send(
            topic=kafka_topic.create_project,
            value=create_render_data,
        )
        logger.info(
            "Sent message to kafka, topic=%s, width=%s, height=%s, samples=%s, denoiser=%s, gpu=%s",  # noqa: E501
            kafka_topic.create_project,
            create_render_data.width,
            create_render_data.height,
            create_render_data.samples,
            create_render_data.denoiser,
            create_render_data.gpu,
        )
