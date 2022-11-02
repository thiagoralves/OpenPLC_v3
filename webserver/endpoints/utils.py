def createRedirectUrl(endpoint, params):
    url = f"/{endpoint}?"

    for k, v in params.items():
        s = f"{k}={v}?"
        url += s

    return url[:-1]
