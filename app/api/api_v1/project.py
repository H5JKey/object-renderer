from dependencies.annotations import (
    AuthUserByAccessTokenDep,
    ProjectServiceDep,
    RenderServiceDep,
)
from fastapi import APIRouter, status
from schemas.project import (
    ProjectResponseList,
    ProjectWithRenderCreate,
    ProjectWithRenderResponse,
)

router = APIRouter(
    prefix="/project",
    tags=["Project"],
)


@router.get(
    "/{project_id}",
    response_model=ProjectWithRenderResponse,
    status_code=status.HTTP_200_OK,
)
async def get_project(
    project_id: int,
    user_id: AuthUserByAccessTokenDep,
    project_service: ProjectServiceDep,
) -> ProjectWithRenderResponse:
    return await project_service.get_by_id(
        project_id=project_id,
        user_id=user_id,
    )


@router.get(
    "/",
    response_model=ProjectResponseList,
    status_code=status.HTTP_200_OK,
)
async def get_current_user_projects(
    project_service: ProjectServiceDep,
    user_id: AuthUserByAccessTokenDep,
    size: int = 10,
    page: int = 1,
) -> ProjectResponseList:
    return await project_service.get_user_projects(
        user_id=user_id,
        size=size,
        page=page,
    )


@router.get(
    "/public/user/{user_id}",
    response_model=ProjectResponseList,
    status_code=status.HTTP_200_OK,
)
async def get_user_public_projects(
    project_service: ProjectServiceDep,
    user_id: int,
    size: int = 10,
    page: int = 1,
) -> ProjectResponseList:
    return await project_service.get_user_public_projects(
        user_id=user_id,
        size=size,
        page=page,
    )


@router.post(
    "/create",
    response_model=ProjectWithRenderResponse,
    status_code=status.HTTP_201_CREATED,
)
async def create_project(
    user_id: AuthUserByAccessTokenDep,
    create_project_data: ProjectWithRenderCreate,
    project_service: ProjectServiceDep,
    render_service: RenderServiceDep,
) -> ProjectWithRenderResponse:
    return await project_service.create_project(
        user_id=user_id,
        create_project=create_project_data,
        render_service=render_service,
    )


@router.delete(
    "/delete/{project_id}",
    status_code=status.HTTP_204_NO_CONTENT,
)
async def delete_project(
    project_id: int,
    user_id: AuthUserByAccessTokenDep,
    project_service: ProjectServiceDep,
) -> None:
    return await project_service.delete_by_id(
        project_id=project_id,
        user_id=user_id,
    )
