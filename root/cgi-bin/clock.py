
import datetime
now = datetime.datetime.now()
time = now.strftime("%A %d %B %Y - %H:%M:%S")

body = f'''\
<!DOCTYPE html>
<html lang="fr">
    <head>
      <meta charset="UTF-8">
      <title>WBSRV time</title>
    </head>
    <body>
       <h1>WEBSERV TIME</h1>
        <h3>{time}<h3>
        <p> <img src="../img/7Z3o.gif" alt="MUCH WOW" /></p>
    </body>
</html>
'''

length = len(body)

print(f"HTTP/1.1 200 OK\r")
print(f"Content-Type: text/html\r")
print(f"Content-Length: {length}\r\n\r")
print(body)