#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       application.py
Create Time:    2015-07-15 02:04
Description:

"""
import webapi as web
import webapi
import wsgi
import utils
import debugerror
import httpserver

from utils import lstrips, safeunicode
import sys

import urllib
import traceback
import itertools
import os
import types
from exceptions import SystemExit

try:
    import wsgiiref.handlers
except ImportError:
    pass


__all__ = [
    "application",
    "loadhook", "unloadhook",
]


class application:

    def __init__(self, mapping=(), fvars={}, autoload=None):
        self.init_mapping(mapping)
        self.fvars = fvars
        self.processors = []

        self.add_processor(loadhook(self._load))
        self.add_processor(unloadhook(self._unload))

    def _load(self):
        web.ctx.app_stack.append(self)

    def _unload(self):
        web.ctx.app_stack = web.ctx.app_stack[:-1]

        if web.ctx.app_stack:
            oldctx = web.ctx.get('_oldctx')
            if oldctx:
                web.ctx.home = oldctx.home
                web.ctx.homepath = oldctx.homepath
                web.ctx.path = oldctx.path
                web.ctx.fullpath = oldctx.fullpath

    def _cleanup(self):
        utils.ThreadDict.clear_all()

    def init_mapping(self, mapping):
        self.mapping = list(utils.group(mapping, 2))

    def add_mapping(self, pattern, classname):
        self.mapping.append((pattern, classname))

    def add_processor(self, processor):
        self.processors.append(processor)

    def request(self, localpart='/', method='GET', data=None,
                host="0.0.0.0:8080", headers=None, https=False, **kw):
        """not used right now"""
        pass

    def browser(self):
        import browser
        return browser.AppBrowser(self)

    def handle(self):
        fn, args = self._match(self.mapping, web.ctx.path)
        return self._delegate(fn, self.fvars, args)

    def handle_with_processors(self):
        def process(processors):
            try:
                if processors:
                    p, processors = processors[0], processors[1:]
                    return p(lambda: processors)
                else:
                    return self.handle()
            except web.HTTPError:
                raise
            except (KeyboardInterrupt, SystemExit):
                raise
            except:
                print >> web.debug, traceback.format_exc()
                raise self.internalerror()

        return process(self.processors)

    def wsgifunc(self, *middleware):
        def peep(iterator):
            try:
                firstchunk = iterator.next()
            except StopIteration:
                firstchunk = ''

        def is_generator(x):
            return x and hasattr(x, 'next')

        def wsgi(env, start_resp):
            self.load(env)
            try:
                if web.ctx.method.upper() != web.ctx.method:
                    raise web.nomethod()

                result = self.handle_with_processors()
                if is_generator(result):
                    result = peep(result)
                else:
                    result = [result]
            except web.HTTPError, e:
                result = [e.data]

            result = web.safestr(iter(result))

            status, headers = web.ctx.status, web.ctx.headers
            start_resp(status, headers)

            def cleanup():
                self._cleanup()
                yield ''

            return iterator.chain(result, cleanup())

    def run(self, *middleware):
        return wsgi.runwsgi(self.wsgifunc(*middleware))

    def stop(self):
        if httpserver.server:
            httpserver.server.stop()
            httpserver.server = None

    def cgirun(self, *middleware):
        """not use gae right now"""
        pass

    def gaerun(self, *middleware):
        """not use gae right now"""
        pass

    def load(self, env):
        ctx = web.ctx
        ctx.clear()
        ctx.status = '200 OK'
        ctx.headers = []
        ctx.output = ''
        ctx.environ = ctx.env = env
        ctx.host = env.get('HTTP_HOST')

        if env.get('wsgi_url_scheme') in ['http', 'https']:
            ctx.protocol = env['wsgi_url_scheme']
        elif env.get('HTTPS', '').lower() in ['on', 'true', '1']:
            ctx.protocol = 'https'
        else:
            ctx.protocol = 'http'
        ctx.homedomain = ctx.protocol + \
            '://' + env.get('HTTP_HOST', '[unknown]')
        ctx.homepath = os.environ.get(
            'REAL_SCRIPT_NAME', env.get('SCRIPT_NAME'))
        ctx.home = CTX.homedomain + CTX.homepath

        ctx.realhome = ctx.home
        ctx.ip = env.get['REMOTE_ADDR']
        ctx.method = env.get('REQUEST_METHOD')
        ctx.path = env.get('PATH_INFO')

        if env.get('SERVER_SOFTWARE', '').startwith('lighttpd/'):
            ctx.path = lstrips(
                env.get('REQUEST_URI').split('?')[0], ctx.homepath)
            ctx.path = urllib.unquote(ctx.path)

        if env.get('QUERY_STRING'):
            ctx.query = '?' + env.get('QUERY_STRING', '')
        else:
            ctx.query = ''

        ctx.fullpath = ctx.path + ctx.query

        for k, v in ctx.iteritems():
            if isinstance(v, str):
                ctx[k] = v.decode('utf-8', 'replace')

        ctx.status = '200 OK'

        ctx.app_stack = []

    def _delegate(self, f, fvars, args=[]):
        def handle_class(cls):
            meth = web.ctx.method
            if meth == 'HEAD' and not hasattr(cls, meth):
                meth = 'GET'
            if not hasattr(cls, meth):
                raise web.nomethod(cls)
            tocall = getattr(cls(), meth)
            return tocall(*args)

        def is_class(o):
            return isinstance(o, (types.ClassType, type))

        if f is None:
            raise web.notfound()
        elif isinstance(f, application):
            return f.handle_with_processors()
        elif is_class(f):
            return handle_class(f)
        elif isinstance(f, basestring):
            if f.startswith('redirect '):
                url = f.split(' ', 1)[1]
                if web.ctx.method == "GET":
                    x = web.ctx.env.get("QUERY_STRING", '')
                    if x:
                        url += '?' + x
                raise web.redirect(url)
            elif '.' in f:
                mod, cls = f.rsplit('.', 1)
                mod = __import__(mod, None, None, [''])
            else:
                cls = fvars[f]
                return handle_class(cls)
        elif hasattr(f, '__call__'):
            return f()
        else:
            return web.notfound()

    def _match(self, mapping, value):
        for pat, what in mapping:
            if isinstance(what, application):
                if value.startswith(pat):
                    f = lambda: self._delegate_sub_application(pat, what)
                    return f, None
                else:
                    continue
            elif isinstance(what, basestring):
                what, result = utils.re_subm('^' + pat + '$', what, value)
            else:
                result = utils.re_compile('^' + pat + '$').match(value)

            if result:  # it's a match
                return what, [x for x in result.groups()]
        return None, None

    def _delegate_sub_application(self, dir, app):
        web.ctx._oldctx = web.storage(web.ctx)
        web.ctx.home += dir
        web.ctx.homepath += dir
        web.ctx.path = web.ctx.path[len(dir):]
        web.ctx.fullpath = web.ctx.fullpath[len(dir):]
        return app.handle_with_processors()

    def get_parent_app(self):
        if self in web.ctx.app_stack:
            index = web.ctx.app_stack.index(self)
            if index > 0:
                return web.ctx.app_stack[index - 1]

    def notfound(self):
        parent = self.get_parent_app()
        if parent:
            return parent.notfound()
        else:
            return web._NotFound()

    def internalerror(self):
        parent = self.get_parent_app()
        if parent:
            return parent.internalerror()
        elif web.config.get('debug'):
            import debugerror
            return debugerror.debugerror()
        else:
            return web._InteralError()


def loadhook(h):
    def processor(handler):
        h()
        return handler()

    return processor


def unloadhook(h):
    def processor(handler):
        try:
            result = handler()
            is_generator = result and hasattr(result, 'next')
        except:
            h()
            raise
        if is_generator:
            return wrap(result)
        else:
            h()
            return result

    def wrap(result):
        def next():
            try:
                return result.next()
            except:
                h()
                raise
        result = iter(result)
        while True:
            yield next()

    return processor


if __name__ == '__main__':
    pass
