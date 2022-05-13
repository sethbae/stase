
"""Some file paths used for the engine and for talking to it"""
ENV_FILE = "env"
ENGINE_USERNAME = "queen_stase_approx"

"""Base URL for hitting the lichess api"""
API_BASE: str = "https://lichess.org/api"


class InvalidTokenException(Exception):
    pass


def read_access_token() -> str:
    """
    Reads the lichess personal access token required to access stase's account.
    """
    try:
        with open(ENV_FILE, "r") as file:
            token = file.read()
    except FileNotFoundError:
        raise InvalidTokenException("Could not read env file.")

    if not token.strip():
        raise InvalidTokenException("Auth token is not valid.")

    return token.strip()
