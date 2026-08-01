from types import TracebackType
from typing import Self, cast

from core.constants import ProjectVisibility
from core.exceptions.auth import PermissionDeniedError
from core.exceptions.file import FileIdNotFoundError
from core.exceptions.project import ProjectIdNotFoundError
from core.exceptions.user import UserIdNotFoundError
from core.interfaces.clients import AbstractUnitOfWorkClient
from core.interfaces.services import AbstractRenderService
from infrastructure.database.models import User
from infrastructure.database.repositories.file import FileRepository
from infrastructure.database.repositories.project import ProjectRepository
from infrastructure.database.repositories.render import RenderRepository
from infrastructure.database.repositories.user import UserRepository
from schemas.file import FileData, FileResponse
from schemas.project import (
    ProjectPartialUpdate,
    ProjectResponse,
    ProjectResponseList,
    ProjectWithRenderAndFileResponse,
    ProjectWithRenderCreate,
    ProjectWithRenderResponse,
)
from schemas.render import GenerateRenderEvent, RenderResponse, UploadRenderProjectEvent


class ProjectService:
    def __init__(
        self,
        unit_of_work: AbstractUnitOfWorkClient,
    ) -> None:
        self.unit_of_work = unit_of_work
        self.user_repository = self.unit_of_work.get_repository(UserRepository)
        self.project_repository = self.unit_of_work.get_repository(ProjectRepository)
        self.render_repository = self.unit_of_work.get_repository(RenderRepository)
        self.file_repository = self.unit_of_work.get_repository(FileRepository)

    async def __aenter__(self) -> "Self":
        return self

    async def __aexit__(
        self,
        exc_type: type[BaseException] | None,
        exc_value: BaseException | None,
        traceback: TracebackType | None,
    ) -> None:
        """
        Метод для действий при выходе из контекстного менеджера.
        """

    async def get_by_id(
        self,
        project_id: int,
        user_id: int,
    ) -> ProjectWithRenderAndFileResponse:
        project = await self.project_repository.get_by_id(project_id)
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        get_owner_coroutine = self.project_repository.get_project_owner(
            project_id,
        )
        owner = cast(User, await get_owner_coroutine)
        if (
            project.visibility == ProjectVisibility.private.value
            and owner.id != user_id
        ):
            detail = "You are not allowed to watch this project."
            raise PermissionDeniedError(detail)

        return ProjectWithRenderAndFileResponse.model_validate(project)

    async def get_user_projects(
        self,
        user_id: int,
        size: int,
        page: int,
    ) -> ProjectResponseList:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        get_projects = self.project_repository.get_user_projects(
            user_id,
        )
        projects = [
            ProjectResponse.model_validate(project) for project in await get_projects
        ]
        return ProjectResponseList(
            project_list=projects,
            size=size,
            page=page,
        )

    async def get_user_public_projects(
        self,
        user_id: int,
        size: int,
        page: int,
    ) -> ProjectResponseList:
        user = await self.user_repository.get_by_id(user_id)
        if user is None:
            raise UserIdNotFoundError(user_id)

        get_projects = self.project_repository.get_user_public_projects(
            user_id,
        )
        projects = [
            ProjectResponse.model_validate(project) for project in await get_projects
        ]
        return ProjectResponseList(
            project_list=projects,
            size=size,
            page=page,
        )

    async def upload_render_to_project(
        self,
        uploaded_render_project_event: UploadRenderProjectEvent,
    ) -> None:
        render_file = uploaded_render_project_event.file
        project_id = uploaded_render_project_event.project_id
        file = await self.file_repository.create_file(render_file)
        project = await self.project_repository.get_by_id(project_id)
        file_response = FileResponse.model_validate(file)
        project_response = ProjectWithRenderAndFileResponse.model_validate(project)
        await self.render_repository.add_render_file(
            render_id=project_response.render.id,
            file_id=file_response.id,
        )

    async def update_project_status(
        self,
        project_id: int,
    ) -> ProjectWithRenderAndFileResponse:
        project = await self.project_repository.update_project_status(project_id)
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        return ProjectWithRenderAndFileResponse.model_validate(project)

    async def create_project(
        self,
        user_id: int,
        create_project: ProjectWithRenderCreate,
        render_service: AbstractRenderService,
    ) -> ProjectWithRenderResponse:
        create_project_data = create_project.project
        create_render_data = create_project.render
        file_id = create_project_data.source_file_id
        file = await self.file_repository.get_by_id(file_id)
        if file is None:
            raise FileIdNotFoundError(file_id)
        file_response = FileData.model_validate(file)
        render = await self.render_repository.create_render(create_render_data)
        project = await self.project_repository.create_project(
            user_id=user_id,
            render_id=render.id,
            create_project_data=create_project_data,
        )
        render_response = RenderResponse.model_validate(render)
        project_response = ProjectResponse.model_validate(project)
        project_with_render_response = ProjectWithRenderResponse(
            render=render_response,
            **project_response.model_dump(),
        )
        render_event_data = GenerateRenderEvent(
            bucket=file_response.bucket,
            key=file_response.key,
            project_id=project_response.id,
            **create_render_data.model_dump(),
        )
        await render_service.send_event_render_model(render_event_data)
        return project_with_render_response

    async def partial_update_project(
        self,
        project_id: int,
        user_id: int,
        partial_update_project_data: ProjectPartialUpdate,
    ) -> ProjectWithRenderAndFileResponse:
        project = await self.project_repository.get_by_id(
            project_id,
        )
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        get_owner_coroutine = self.project_repository.get_project_owner(
            project_id,
        )
        owner = cast(User, await get_owner_coroutine)
        if owner.id != user_id:
            detail = "You are not allowed to watch this project."
            raise PermissionDeniedError(detail)

        project = await self.project_repository.partial_update_project(
            project_id,
            partial_update_project_data,
        )
        return ProjectWithRenderAndFileResponse.model_validate(project)

    async def delete_by_id(
        self,
        project_id: int,
        user_id: int,
    ) -> None:
        project = await self.project_repository.get_by_id(
            project_id,
        )
        if project is None:
            raise ProjectIdNotFoundError(project_id)

        get_owner_coroutine = self.project_repository.get_project_owner(
            project_id,
        )
        owner = cast(User, await get_owner_coroutine)
        if owner.id != user_id:
            detail = "You are not allowed to watch this project."
            raise PermissionDeniedError(detail)

        await self.project_repository.delete_by_id(project_id)
