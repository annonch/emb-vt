# Management Application for Smart Grid System

Using Python and ZMQ with [pyzmq](https://learning-0mq-with-pyzmq.readthedocs.io/en/latest/pyzmq/pyzmq.html)

> Use **vtmanager.py** for now

### How to use

```
# for debug mode
python vtmanager.py
# for release mode
python -o vtmanager.py
```

`consumers.txt` contains all the sensors' IP.

e.g. `tcp://127.0.0.1:5555`
