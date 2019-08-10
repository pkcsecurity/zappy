import urllib as ulib
import urllib.request as req
import time


def _get(url):
    resp = req.urlopen(url, None, 5)
    resp_bytes = resp.read()
    if isinstance(resp_bytes, bytes):
        resp_str = resp_bytes.decode("utf-8")
    elif isinstance(resp_bytes, str):
        resp_str = resp_bytes
    return resp_str.strip()


def zap():
    try:
        print(_get("http://10.59.1.131"))
    except BaseException as be:
        print(f"Failed to zap: {be}")


req_wpm = 60
char_count = 0
start_time = 0


def update_wpm():
    global char_count, start_time, req_wpm
    if char_count == 0:
        start_time = time.time()
        char_count += 1
        return

    char_count += 1
    cur_time = time.time()
    delta = cur_time - start_time

    # check every 2 seconds
    if delta >= 2:
        if delta >= 7:
            # reset counting if the delay between inserting characters is more
            # than 7 seconds
            char_count = 0
            return
        wpm = (char_count / 5) * (60 / delta)
        if wpm < req_wpm:
            zap()
