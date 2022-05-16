from src.lichess.engine_client import EngineClient
from src.lichess.client import (
    stream_game_events,
    make_move,
    resign_game,
    post_to_chat
)
from src.lichess.info import (
    ENGINE_USERNAME
)

THINK_TIME_PROPORTION: float = 0.05


def post_update(token: str, game_id: str, engine: EngineClient, requested_think_time: float) -> None:
    """
    Posts an update to the lichess chat with the engine's speed + evaluation.
    """
    if engine.get_time_elapsed() != 0:
        speed = int(engine.get_node_count() / engine.get_time_elapsed())
    else:
        speed = int(engine.get_node_count() / requested_think_time)
    post_to_chat(token, game_id,
                 f" {engine.get_eval_str()} ({round(engine.get_time_elapsed(), 1)}s @ {speed}n/s)")


def play_game(token: str, game_id: str, chat_updates=False):
    def play_a_move(think_time) -> bool:
        move: str = engine.get_computer_move(think_time)
        if move == "ERROR":
            print("Encountered engine error")
            return False
        elif move == "no move":
            print("No move possible")
            return True
        else:
            if chat_updates:
                post_update(token, game_id, engine, think_time)
            return make_move(token, game_id, move)

    post_to_chat(token, game_id, "Hello there! I'll post regular updates. If you want me to stop, say \"mute\"!")
    engine: EngineClient = EngineClient()

    as_white: bool = False

    for event in stream_game_events(token, game_id):

        if "error" in event:
            print(f"play game for {game_id} received error")
            return

        if event["type"] == "gameFull":

            print("Received full game information")

            as_white = (event["white"]["id"] == ENGINE_USERNAME)

            if as_white:
                if not play_a_move(think_time=5):
                    resign_game(token, game_id)
                    return

        elif event["type"] == "chatLine":
            if event["text"] in ["m", "mute"]:
                chat_updates = False
            elif event["text"] == "unmute":
                chat_updates = True

        elif "status" in event and event["status"] != "started":
            print(f"Game {game_id} has ended")
            return

        elif event["type"] == "gameState":

            move_played: str = event["moves"].split()[-1]
            half_move_count = event["moves"].count(' ') + 1
            millis_remaining = event["wtime"] if as_white else event["btime"]

            if (as_white and half_move_count % 2 == 0) \
                    or (not as_white and half_move_count % 2 == 1):
                # update the engine with the opponent's move
                engine.register_opponent_move(move_played)
                # play a move of our own
                if not play_a_move(int((millis_remaining * THINK_TIME_PROPORTION) / 1000)):
                    print("Error encountered: resigning the game")
                    resign_game(token, game_id)
                    return
        else:
            print(f"Received event of type {event['type']}")
