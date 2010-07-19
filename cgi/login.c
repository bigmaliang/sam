#include "mheads.h"
#include "ouser.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
	CGI *cgi = NULL;
	NEOERR *err;
	mdb_conn *conn = NULL;

	/* skey, user, pass, return jsoncallback*/
	char *s, *u, *p, *r, *jcbk;
	/* keeptime(hours)  */
	int t;
	char tm[LEN_TM_GMT];
	
	//sleep(20);
	mtc_init("login");
	mconfig_parse_file(SITE_CONFIG, &g_cfg);
	mutil_wrap_fcgi(argc, argv, envp);
	if (mdb_init(&conn, DB_DSN) != MDB_ERR_NONE) {
		mtc_err("init db error %s", mdb_get_errmsg(conn));
		printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
		printf("{errcode: %d}", SAM_ERR_INIT);
		return 1;
	}
	
#ifndef DROP_FCGI
	while (FCGI_Accept() >= 0) {
#endif

		/*
		 * cgi init 
		 */
		err = cgi_init(&cgi, NULL);
		if (err != STATUS_OK) {
			mtc_err("init cgi error");
			printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
			printf("{errcode: %d}", SAM_ERR_INIT);
			goto opfinish;
		}
		err = cgi_parse(cgi);
		if (err != STATUS_OK) {
			mtc_err("parse cgi error");
			hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_PARSE);
			goto opfinish;
		}
		if (mutil_client_attack_cookie(cgi->hdf, "login", 30, 60)) {
			mtc_err("client attack");
			hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_NEEDREST);
			goto opfinish;
		}

		u = hdf_get_value(cgi->hdf, PRE_COOKIE".samuser", NULL);
		s = hdf_get_value(cgi->hdf, PRE_COOKIE".samkey", NULL);
		if (s && u) {
			if (user_has_login(conn, u, s)) {
				hdf_set_copy(cgi->hdf, PRE_OUTPUT".samuser", PRE_COOKIE".samuser");
				hdf_set_copy(cgi->hdf, PRE_OUTPUT".samkey", PRE_COOKIE".samkey");
				goto done;
			}
		}
		
		u = hdf_get_value(cgi->hdf, PRE_QUERY".u", NULL);
		p = hdf_get_value(cgi->hdf, PRE_QUERY".p", NULL);
		if (!u || !p) {
			mtc_err("parameter miss %s %s", u, p);
			hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_NEEDINPUT);
			goto opfinish;
		}
		s = user_login_auth(conn, u, p);
		if (!s) {
			mtc_err("login error %s %s", u, p);
			hdf_set_int_value(cgi->hdf, PRE_OUTPUT".errcode", SAM_ERR_PASSW);
			goto opfinish;
		}

		cgi_cookie_set(cgi, "samuser", u, NULL, SITE_DOMAIN, NULL, 1, 0);
		cgi_cookie_set(cgi, "samkey", s, NULL, SITE_DOMAIN, NULL, 1, 0);
#if 0
		t = hdf_get_int_value(cgi->hdf, PRE_QUERY".t", 0);
		mmisc_getdatetime_gmt(tm, sizeof(tm), "%A, %d-%b-%Y %T GMT", 60*60*t);
		cgi_cookie_set(cgi, "samkey", s, NULL, SITE_DOMAIN, tm, 1, 0);
#endif
		
		hdf_set_value(cgi->hdf, PRE_OUTPUT".samuser", u);
		hdf_set_value(cgi->hdf, PRE_OUTPUT".samkey", s);
		free(s);

	done:
		/*
		 * TODO set samkey, samuser to app's domain
		 * DONE this is done by jsonp
		 */
		hdf_set_value(cgi->hdf, PRE_OUTPUT".success", "1");
	opfinish:
		if (cgi) {
			r = hdf_get_value(cgi->hdf, PRE_QUERY".r", NULL);
			if (r) {
				cgi_redirect(cgi, r);
			} else {
				jcbk = hdf_get_value(cgi->hdf, PRE_QUERY".jsoncallback", NULL);
				if (jcbk != NULL) {
					mjson_execute_hdf(cgi->hdf, jcbk, 0);
				} else {
					mjson_output_hdf(cgi->hdf, 0);
				}
			}
#ifdef DEBUG_HDF
			hdf_write_file(cgi->hdf, HF_LOG_PATH"hdf.login");
#endif
			cgi_destroy(&cgi);
		}
		
#ifndef DROP_FCGI
 	} /* FCGI_Accept() */
#endif

	mdb_destroy(conn);
	return 0;
}
