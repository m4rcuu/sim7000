from sim7000 import SIM7000
import random

messages = ['at',
            'at+cmnb=1',
            'at+cnmp=38',
            'at+cgatt=1',
            'at+cstt="pllab.pl"',
            'at+ciicr',
            'at+cifsr',
            'at+cnact=1',
            'at+smconf="URL",172.20.39.40,1883',
            'at+smconf="KEEPTIME",60',
            'at+smconn',
            'at+smsub="home/livingroom/temperature",1',
            'at+smpub="home/livingroom/temperature","2",1,1',
            str(random.randrange(99))]

sim = SIM7000()

output_messages = sim.send_list_messages(messages)

print(output_messages)
