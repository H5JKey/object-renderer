from core.constants import ProjectVisibility
from core.exceptions.auth import PermissionDeniedError
from core.exceptions.render_project import RenderProjectIdNotFoundError
from core.interfaces.repositories import AbstractRenderProjectRepository
from schemas.render_project import RenderProjectCreate, RenderProjectResponse


class RenderProjectService:
    def __init__(
        self,
        render_project_repository: AbstractRenderProjectRepository,
    ) -> None:
        self.render_project_repository = render_project_repository

    async def get_by_id(
        self,
        render_project_id: int,
        user_id: int,
    ) -> RenderProjectResponse:
        render_project = await self.render_project_repository.get_by_id(
            render_project_id,
        )
        if render_project is None:
            raise RenderProjectIdNotFoundError(render_project_id)

        owner = await self.render_project_repository.get_render_project_owner(
            render_project_id,
        )

        if (
            render_project.visibility == ProjectVisibility.private.value
            and owner.id != user_id
        ):
            raise PermissionDeniedError("You are not allowed to watch this project.")

        return RenderProjectResponse.model_validate(render_project)

    async def create_render_project(
        self,
        user_id: int,
        create_render_project: RenderProjectCreate,
    ) -> RenderProjectResponse:
        render_project = await self.render_project_repository.create_render_project(
            user_id=user_id,
            create_render_project_data=create_render_project,
        )
        return RenderProjectResponse.model_validate(render_project)

    async def delete_by_id(
        self,
        render_project_id: int,
        user_id: int,
    ) -> None: ...
