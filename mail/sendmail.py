#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       sendmail.py
Create Time:    2015-07-08 00:47
Description:
    send mail program
Todo:
    bug fix
"""
import argparse
import logging
import smtplib
import sys

from email.mime.text import MIMEText


def config_log():
    logger = logging.getLogger('out')
    logger.setLevel(logging.DEBUG)

    fh = logging.FileHandler("./httpd.log")
    fh.setLevel(logging.INFO)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    formatter_full = logging.Formatter(
        '%(asctime)s - %(process)d - %(levelname)s - %(filename)s:%(lineno)d - %(message)s')
    ch.setFormatter(formatter_full)
    fh.setFormatter(formatter)

    logger.addHandler(ch)
    # logger.addHandler(fh)

    return logger

def compact_tracebak():
    """format exception output"""
    t, v, tb = sys.exc_info()
    tbinfo = []
    if not tb:
        raise AssertionError("traceback does not exist")
    while tb:
        tbinfo.append((
            tb.tb_frame.f_code.co_filename,
            tb.tb_frame.f_code.co_name,
            str(tb.tf_lineno),
        ))
        tb = tb.tb_next

    del tb
    file, funcion, line = tbinfo[-1]
    info = ' '.join('[%s|%s|%s]' % x for x in tbinfo)
    return (file, funcion, line), t, v, info



def parse_argument():
    parser = argparse.ArgumentParser(
        description="sendmail tool, default sender xuechao8086@qq.com",
        epilog="""example: %(prog)s\
                -s xuechao8086@qq.com\
                -p send's password\
                -r xuechao8086@126.com\
                -t "a mail from charliezhao"\
                -c /tmp/hello.html, /tmp/obj.html""")

    parser.add_argument(
        "-s", "--sender",
        type=str, default="xuechao8086",
        metavar="sender",
        help="xuechao8086@qq.com default sender")

    parser.add_argument(
        "-p", "--password",
        type=str, metavar="password",
        help="password", required=True)

    parser.add_argument(
        "-r", "--receiver",
        type=str, metavar="mail receivers",
        nargs="+",
        help="mail receiver list", required=True)

    parser.add_argument(
        "-t", "--title",
        type=str, metavar="title",
        default="mail from charliezhao",
        help="mail titile")

    parser.add_argument(
        "-c", "--content",
        type=file, metavar="file",
        help="content file path")

    return parser.parse_args()


class SendMail(object):

    def __init__(self, sender, pwd, receiver_list, title, fd):
        self.sender = sender
        self.pwd = pwd
        self.receiver_list = receiver_list
        self.title = title
        self.content = fd.read() 

        self.server = "smtp.qq.com"
        self.port = 465
        
        fd.close()

    def send_mail(self):
        msg = MIMEText(self.content, _subtype="html", _charset="utf-8")
        msg["Subject"] = self.title
        msg["From"] = self.sender
        msg["To"] = ";".join(self.receiver_list)

#        try:
#            s = smtplib.SMTP()
#            s.connect(self.server)
#            s.sendmail(self.sender, self.receiver_list, msg.as_string)
#            s.close()
#            logger.info("Send OK")
#            return True
#        except Exception, e:
#            logger.debug(compact_tracebak())
#            logger.error("Send Fail")
#            return False
#
        s = smtplib.SMTP()
        s.connect(self.server)
        s.login(self.sender,  self.pwd)
        s.sendmail(self.sender, self.receiver_list, msg.as_string)
        s.close()
        logger.info("Send OK")
        return True


def send_mail(sender, pwd, receiver_list, title, fd):
        me = "hello" + "<" + sender + "@" + "qq.com" + ">"
        msg = MIMEText(fd.read(), _subtype="html", _charset="utf-8")
        msg["Subject"] = title
        msg["From"] = sender
        msg["To"] = ";".join(receiver_list)
        
        s = smtplib.SMTP()
        s.connect("smtp.qq.com")
        s.login(sender, pwd)
        s.sendmail(sender, receiver_list, msg.as_string())
        s.close()
        logger.info("Send OK")
        return True


if __name__ == "__main__":
    logger = config_log()
    args = parse_argument()

    mail_sender = args.sender
    mail_password = args.password
    mail_receiver_list = args.receiver
    mail_title = args.title
    mail_fd = args.content
        
#    SendMail(mail_sender, mail_password,
#            mail_receiver_list, mail_title, mail_fd).send_mail()

    send_mail(mail_sender, mail_password,
            mail_receiver_list, mail_title, mail_fd)
