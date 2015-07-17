#web.py 备忘
===========
* 遵循**wsgi**规定
* 和请求相关的信息， 都存储在web.ctx变量中， 该变量是线程安全的， thread local data.
* 内部使用了较多的闭包函数，另外还有递归，关于递归， 有如下备忘:
  *	python stack 递归深度， 不超过`1K`个。
  *	C stack 递归深度，为`26w`。
  * 以上测试在个人计算机上进行。
* 请求路径

	```
	-> application.py application.run
	-> wsgi.py wsgi.runwsgi
	-> httpserver.py runsimple
	-> httpserver.py StaticMiddleware
	-> httpserver.py LogMiddleware
	-> application.py application.wsgifunc
	
	# recursive calls
	-> application.py application.handle_with_processors 
	
	# return an iterator for applicaion process's content.
	-> application.py application.handle
	
	# application route and executor.
	-> application.py application._delegate
	
	# for wsgi specification， write out headers, then contents.
	-> httpserver.py 

	```
* python23.py 实现了一个thread local data.
* utils.py 在python23.py 基础上实现了TreadedDict, 并提供了其他的实用函数：
  *	Storage类 dict的一个子类
  * Counter类 collections中Counter类似实现
  * timelimit A decorator to limit a function to `timeout` seconds
  * Memoize 类 Memoizes' a function, caching its return values for each input.
  * 其他实用函数
* net.py 提供了一些ip地址正确性验证函数，并对html特殊字符进行了翻译
* webapi.py 对404、500等html 返回码进行了实例化。
* 对http刷新请求， 对比请求时间，和etag, 对比符合要求，则直接返回304 节约流量。
* 假如浏览器支持， 则启用如gzip之类的压缩， 节约带宽。
* browser.py 提供了一个http请求的模拟。 
