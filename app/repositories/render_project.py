from core.constants import ProjectVisibility
from core.interfaces.repositories import AbstractRenderProjectRepository
from models import RenderProject, User
from schemas.render_project import RenderProjectCreate
from sqlalchemy import and_, delete, select
from sqlalchemy.ext.asyncio import AsyncSession


class RenderProjectRepository(AbstractRenderProjectRepository):
    def __init__(self, session: AsyncSession) -> None:
        self.session = session

    async def get_by_id(self, render_project_id: int) -> RenderProject | None:
        return await self.session.get(
            entity=RenderProject,
            ident=render_project_id,
        )

    async def get_render_project_owner(self, render_project_id: int) -> User | None:
        stmt = (
            select(User)
            .join(User.render_projects)
            .where(RenderProject.id == render_project_id)
        )
        result = await self.session.execute(stmt)
        return result.scalar()

    async def get_user_render_projects(self, user_id: int) -> list[RenderProject]:
        stmt = select(RenderProject).where(RenderProject.user_id == user_id)
        result = await self.session.execute(stmt)
        return list(result.scalars().all())

    async def get_user_public_render_projects(
        self,
        user_id: int,
    ) -> list[RenderProject]:
        stmt = select(RenderProject).filter(
            and_(
                RenderProject.user_id == user_id,
                RenderProject.visibility == ProjectVisibility.public.value,
            ),
        )
        result = await self.session.execute(stmt)
        return list(result.scalars().all())

    async def create_render_project(
        self,
        user_id: int,
        create_render_project_data: RenderProjectCreate,
    ) -> RenderProject:
        render_project = RenderProject(
            user_id=user_id,
            **create_render_project_data.model_dump(),
        )
        self.session.add(render_project)
        await self.session.commit()
        await self.session.refresh(render_project)
        return render_project

    async def delete_by_id(self, render_project_id: int) -> None:
        stmt = delete(RenderProject).where(RenderProject.id == render_project_id)
        await self.session.execute(stmt)
        await self.session.commit()
