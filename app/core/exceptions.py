class InvalidTokenPayloadError(Exception):
    """
    Ошибка при неправильном payload токена.
    """

    def __init__(self, payload: dict[str, str]) -> None:
        self.payload = payload
        detail = f"Invalid Token Payload: {payload}"
        super().__init__(detail)
