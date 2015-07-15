#!/usr/bin/env python

#import sys
#sys.path.append("/Users/charlie/Documents/webpy/")


from application import application

urls = (
    "/(.*)", "hello"
)

#print "globals() = {}".format(pprint.pprint(globals()))
app = application(urls, globals())

class hello(object):
    def GET(self, name):
        if not name:
            name = "World"
        return "Hello, " + name + "!"

if __name__ == "__main__":
    app.run()
