from core.exceptions.base import NotFoundError


class RenderProjectNotFoundError(NotFoundError):
    """
    Ошибка, связанная с ненахождением проекта.
    """

    def __init__(self, detail: str) -> None:
        super().__init__(detail)


class RenderProjectIdNotFoundError(RenderProjectNotFoundError):
    """
    Ошибка, связанная с ненахождением id проекта.
    """

    def __init__(self, render_project_id: int) -> None:
        self.render_project_id = render_project_id
        detail = f"Render project with id = {render_project_id} not found."
        super().__init__(detail)
