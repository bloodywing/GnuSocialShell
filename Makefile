CC = gcc
OBJ_NAME = gnusocialshell
OBJS = Main.c loadConfig.c lib/get_account_info.c lib/gnusocial.c gnusocialshell.c lib/send_status.c lib/favorite.c lib/search_by_id.c lib/delete_status_by_id.c lib/answer_status_by_id.c lib/read_timeline.c lib/retweet_by_id.c lib/get_user_info_by_id.c lib/print_users_array_info.c lib/verify_account.c lib/get_group_info.c lib/join_group.c lib/leave_group.c lib/list_groups.c lib/follow_user.c lib/unfollow_user.c lib/send_to_api.c
FLAGS = -Wall -lcurl
LIBCURL = -lcurl

all:
	$(CC) $(FLAGS) $(OBJS) -o $(OBJ_NAME) $(LIBCURL)
install:
	$(CC) $(FLAGS) $(OBJS) -o $(OBJ_NAME) $(LIBCURL)
	rm -rf $(HOME)/.config/gnusocialshell
	mkdir $(HOME)/.config/gnusocialshell
	install -Dm644 config $(DESTDIR)$(HOME)/.config/gnusocialshell/gnusocialshell.conf
	install -Dm755 gnusocialshell $(DESTDIR)/usr/bin/gnusocialshell
	chmod -R 777 $(HOME)/.config/gnusocialshell
clean:
	rm $(OBJ_NAME)
