from core.interfaces.repositories import AbstractRenderRepository
from schemas.render import RenderCreate
from sqlalchemy.ext.asyncio import AsyncSession

from infrastructure.database.models import Render


class RenderRepository(AbstractRenderRepository):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def create_render(self, create_render_data: RenderCreate) -> Render:
        render = Render(**create_render_data.model_dump())
        self.session.add(render)
        await self.session.flush()
        return render
