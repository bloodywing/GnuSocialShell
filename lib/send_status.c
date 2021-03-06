/*
 * Copyright (C) 2016, 2017 Daniel Martín
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gnusocial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

extern int loglevel;

void send_status(struct gss_account account, char *msg)
{
        /* cURL functionality used just to URIencode the msg */
        CURL *curl = curl_easy_init();
	if(curl) {
                char *encoded_msg = curl_easy_escape(curl, msg, strlen(msg));
		if(encoded_msg) {
                        int amount = 31+strlen(encoded_msg);
			char *send = malloc(amount);
			snprintf(send, amount, "source=GnuSocialShell&status=%s", encoded_msg);
			if (loglevel >= LOG_DEBUG) { // OK?
			        fprintf(stderr, "source=GnuSocialShell&status=%s", encoded_msg);
			}
			char *xml_data = send_to_api(account, send, "statuses/update.xml");
			int xml_data_size = strlen(xml_data);
			if (FindXmlError(xml_data, strlen(xml_data)) < 0 && parseXml(xml_data, xml_data_size, "</status>", 9, NULL, 0) > 0) {
				struct status posted_status;
				posted_status = makeStatusFromRawSource(xml_data, xml_data_size);
				print_status(posted_status);
			}
			free(xml_data);
			free(send);
			curl_free(encoded_msg);
		}
	}
}
