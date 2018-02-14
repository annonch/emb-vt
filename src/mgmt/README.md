# Management Application for Smart Grid System
Using Python with [pyzmq](https://learning-0mq-with-pyzmq.readthedocs.io/en/latest/pyzmq/pyzmq.html)

> Use **vtmanager.py** for now
```
# for debug mode
python vtmanager.py
# for release mode
python -o vtmanager.py
```

### How to use
```
python vtresultcollector.py
python vtconsumer.py -i 1 -p 1234
python vtproducer.py -f consumers.txt -o STOP -s 1
```
> **vtresultcollector.py** and **vtproducer.py** belong to the master node, **vtconsumer.py** belongs to the sensors (slaves) nodes

#### vtproducer.py
| Args           | Value                                      |
|:--------------:| ------------------------------------------ |
| '-f' '--file'  | File which contains all the consumers IPs  |
| '-o' '--option'| Option for sending out to all the consumers|
| '-s' '--sensor'| The ID of the sensor                       |
| '-h' '--help'  | Help                                       |

#### vtconsumer.py
| Args             | Value                      |
|:----------------:| -------------------------- |
| '-i' '--id'      | ID of this sensor          |
| '-m' '--masterip'| IP of the Master Controller|
| '-p' '--pid'     | Process ID                 |
| '-h' '--help'    | Help                       |

#### vtresultcollector.py
| Args           | Value                                      |
|:--------------:| ------------------------------------------ |
| '-f' '--file'  | File which contains all the consumers IPs  |
| '-o' '--option'| Option for sending out to all the consumers|
| '-h' '--help'  | Help                                       |

`consumers.txt` contains all the sensors' IP.

e.g. `tcp://127.0.0.1:5555`
