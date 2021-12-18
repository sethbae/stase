
"""Some file paths used for the engine and for talking to it"""
GAME_FILE_DIR = "/home/seth/Documents/stase_lichess"
STASE_SRC_DIR = "/home/seth/Documents/stase_lichess/stase"
ENGINE_USERNAME = "queen_stase_approx"

"""Base URL for hitting the lichess api"""
API_BASE: str = "https://lichess.org/api"


def read_access_token() -> str:
    """
    Reads the lichess personal access token required to access stase's account.
    """
    with open(".env", "r") as file:
        token = file.read()

    return token.strip()
