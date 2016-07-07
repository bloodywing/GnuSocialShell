/*
 * Copyright (C) 2016 Dan Rulos.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "loadConfig.h"
#include "lib/gnusocial.h"
#include "gnusocialshell.h"

#define VERSION "0.7"
#define MAX_PATH 128
#define _FALSE 0
#define _TRUE 1
#define ALL_OK 0

#define FAVORITES "favorites.xml"
#define PUBLIC_TIMELINE "statuses/public_timeline.xml"
#define HOME_TIMELINE "statuses/home_timeline.xml"
#define MENTIONS "statuses/mentions.xml"

void version();
void help();
void gss_shell();

struct gss_account main_account;

int main(int argc, char **argv)
{
	char config_path[MAX_PATH] = "./config\0";
	int ret = 0;
	int vflag = _FALSE;
	int hflag = _FALSE;
	int fflag = _FALSE;
	int i;
	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--version") == 0) || (strcmp(argv[i], "-v") == 0)) {
			vflag = _TRUE;
		}
		else if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
			hflag = _TRUE;
		}
		else if (((strcmp(argv[i], "--config") == 0) || (strcmp(argv[i], "-c") == 0))) {
			if ((i+1) < argc) {
				strcpy(config_path, argv[i+1]);
				i++;
			}
			else {
				printf("%s: missing a file as an argument\n", argv[0]);
				fflag = _TRUE;
			}
		}
		else {
			printf("%s: unrecognized option '%s'\n", argv[0], argv[i]);
			printf("Try '%s --help' for more information\n", argv[0]);
			fflag = _TRUE;
		}
	}
	if (vflag) {
		version();
	}
	if (hflag) {
		help();
	}
	if (!vflag && !hflag && !fflag) {
		if ((ret = loadConfig(config_path)) == ALL_OK) {
			if (mkdir("temp/", 0777) != -1) {
				printf("Type '/help' to get a list of commands\n\n");
				gss_shell();
			}
			else {
				printf("Error: %s\n", strerror(errno));
			}
			unlink("temp/file.xml");
			rmdir("temp/");
		}
	}
	return ret;
}

void version()
{
	printf("GnuSocialShell v%s\n", VERSION);
}

void help()
{
	printf("Usage: ./gss [OPTION]\n");
	printf("--help, -h\t\tPrints this help\n");
	printf("--version, -v\t\tPrints GSS version\n");
	printf("--config, -c [FILE]\tUse FILE as configuration file\n");
	printf("\nWrote by DalmeGNU (dalmemail _AT_ amaya.tk)\n\n");
}

/* compara una cadena de caracteres desde el caracter 0 hasta que encuentra un espacio en blanco
   o el final de la cadena */
int cmdcmp(char *a, char *b)
{
	int ret = 0;
	int max = strlen(a);
	int i;
	for (i = 0; i < max && a[i] != ' '; i++) {
		if (a[i] != b[i]) {
			ret++;
		}
	}
	return ret;
}

void gss_shell()
{
	extern struct gss_account main_account;
	char cmdline[256];
	char *args;
	int i;
	struct account_info info;
	do {
		printf("@%s@%s-> ", main_account.user, main_account.server);
		fgets(cmdline, 256, stdin);
		cmdline[strlen(cmdline)-1] = '\0';
		if (strlen(cmdline) > 0) {
			if (strcmp(cmdline, "/help") == 0) {
				help_command();
			}
			else if (strcmp(cmdline, "/me") == 0) {
				info = get_account_info(main_account);
				me_command(info);
			}
			else if (cmdcmp(cmdline, "/send") == 0) {
				if (strlen(cmdline) >= 7) {
					args = &cmdline[6];
					send_status(main_account, args);
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/favorite") == 0) {
				if (strlen(cmdline) >= 11) {
					args = &cmdline[10];
					favorite(main_account, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/unfavorite") == 0) {
				if (strlen(cmdline) >= 13) {
					args = &cmdline[12];
					unfavorite(main_account, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/search") == 0) {
				if (strlen(cmdline) >= 9) {
					args = &cmdline[8];
					search_by_id(main_account, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/delete") == 0) {
				if (strlen(cmdline) >= 9) {
					args = &cmdline[8];
					delete_status_by_id(main_account, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/reply") == 0) {
				if (strlen(cmdline) >= 8) {
					args = &cmdline[7];
					int cmdline_size = strlen(cmdline);
					for (i = 7; i < cmdline_size && cmdline[i] != ' '; i++);
					if ((i+1) < cmdline_size) {
						cmdline[i] = '\0';
						char *msg = &cmdline[i+1];
						answer_status_by_id(main_account, atoi(args), msg);
					}
					else {
						printf("Error: Invalid usage, see '/help' for details\n");
					}
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/favorites") == 0) {
				if (strlen(cmdline) >= 12) {
					args = &cmdline[11];
					read_timeline(main_account, FAVORITES, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/mentions") == 0) {
				if (strlen(cmdline) >= 11) {
					args = &cmdline[10];
					read_timeline(main_account, MENTIONS, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/home_timeline") == 0) {
				if (strlen(cmdline) >= 16) {
					args = &cmdline[15];
					read_timeline(main_account, HOME_TIMELINE, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/ht") == 0) {
				if (strlen(cmdline) >= 5) {
					args = &cmdline[4];
					read_timeline(main_account, HOME_TIMELINE, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/public_timeline") == 0) {
				if (strlen(cmdline) >= 18) {
					args = &cmdline[17];
					read_timeline(main_account, PUBLIC_TIMELINE, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (cmdcmp(cmdline, "/pt") == 0) {
				if (strlen(cmdline) >= 5) {
					args = &cmdline[4];
					read_timeline(main_account, PUBLIC_TIMELINE, atoi(args));
				}
				else {
					printf("Error: Invalid usage, see '/help' for details\n");
				}
			}
			else if (strcmp(cmdline, "/quit") != 0 && cmdline[0] == '/') {
				printf("Command '%s' not found\n", cmdline);
			}
		}
	} while(strcmp(cmdline, "/quit") != 0);
}
