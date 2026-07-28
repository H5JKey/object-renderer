from dependencies.annotations import AuthUserByAccessTokenDep, RenderProjectServiceDep
from fastapi import APIRouter, status
from schemas.render_project import (
    RenderProjectCreate,
    RenderProjectResponse,
    RenderProjectResponseList,
)

router = APIRouter(
    prefix="/render-project",
    tags=["Render Project"],
)


@router.get(
    "/{render_project_id}",
    response_model=RenderProjectResponse,
    status_code=status.HTTP_200_OK,
)
async def get_render_project(
    render_project_id: int,
    user_id: AuthUserByAccessTokenDep,
    render_project_service: RenderProjectServiceDep,
) -> RenderProjectResponse:
    return await render_project_service.get_by_id(
        render_project_id=render_project_id,
        user_id=user_id,
    )


@router.get(
    "/",
    response_model=RenderProjectResponseList,
    status_code=status.HTTP_200_OK,
)
async def get_current_user_render_projects(
    render_project_service: RenderProjectServiceDep,
    user_id: AuthUserByAccessTokenDep,
    size: int = 10,
    page: int = 1,
) -> RenderProjectResponseList:
    return await render_project_service.get_user_render_projects(
        user_id=user_id,
        size=size,
        page=page,
    )


@router.get(
    "/{user_id}/public",
    response_model=RenderProjectResponseList,
    status_code=status.HTTP_200_OK,
)
async def get_user_public_render_projects(
    render_project_service: RenderProjectServiceDep,
    user_id: int,
    size: int = 10,
    page: int = 1,
) -> RenderProjectResponseList:
    return await render_project_service.get_user_public_render_projects(
        user_id=user_id,
        size=size,
        page=page,
    )


@router.post(
    "/create",
    response_model=RenderProjectResponse,
    status_code=status.HTTP_201_CREATED,
)
async def create_render_project(
    user_id: AuthUserByAccessTokenDep,
    create_render_project_data: RenderProjectCreate,
    render_project_service: RenderProjectServiceDep,
) -> RenderProjectResponse:
    return await render_project_service.create_render_project(
        user_id=user_id,
        create_render_project=create_render_project_data,
    )


@router.delete(
    "delete/{render_project_id}",
    status_code=status.HTTP_204_NO_CONTENT,
)
async def delete_render_project(
    render_project_id: int,
    user_id: AuthUserByAccessTokenDep,
    render_project_service: RenderProjectServiceDep,
) -> None:
    return await render_project_service.delete_by_id(
        render_project_id=render_project_id,
        user_id=user_id,
    )
