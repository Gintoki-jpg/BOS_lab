//自定义实现计时器(但是现在我还不知道怎么使用，这个计时器实际上就已经完成了实验的一个大部分了，我们现在的目标就是学会玩这个计时器)
#include "main.h"

static struct timer_list *timer_list = NULL;

static void sig_func(int signo)
{
	struct list_head *node;
	struct list_head *tmp;
	timer_node_t *timer;
	if(list_empty(&timer_list->head) == true){
		return;
	}
	list_for_each_safe(node, tmp, &timer_list->head) {
		timer = list_entry(node, struct timer, node);
		timer->elapse++;
                if(timer->elapse >= timer->interval) {
                        timer->elapse = 0;
                        timer->cb(timer->id, timer->user, timer->len);
                }
	}
}

struct timer_list *create_timer_list(int count)
{
        int ret = 0;
	struct timer_list *ptr = NULL;

        if(count <=0 || count > MAX_TIMER_NUM) {
               printf("the timer max number too big, MAX num is %d.\n", MAX_TIMER_NUM);
               return NULL;
        }

	ptr = (struct timer_list *)malloc(sizeof(struct timer_list));
        memset(ptr, 0, sizeof(struct timer_list));
        INIT_LIST_HEAD(&ptr->head);
        ptr->size = count;

        /* Register our internal signal handler and store old signal handler */
        if ((ptr->sighandler_old = signal(SIGALRM, sig_func)) == SIG_ERR) {
                goto err_out;
        }
        ptr->sighandler = sig_func;


        ptr->value.it_value.tv_sec = 0;
        ptr->value.it_value.tv_usec = 1;   /*for firt timeout*/
        ptr->value.it_interval.tv_sec = 0;
        ptr->value.it_interval.tv_usec = 1;/*for tick reload*/
        ret = setitimer(ITIMER_REAL, &ptr->value, &ptr->ovalue);
	if (ret < 0)
		goto err_out;
        return ptr;
err_out:
	printf("create_timer_list error\n");
	free(ptr);
	return NULL;
 }

 int destroy_timer(struct timer_list *list)
 {
        struct timer *node = NULL;

        signal(SIGALRM, list->sighandler_old);
        setitimer(ITIMER_REAL, &list->ovalue, &list->value);

	if(list_empty(&list->head) == false){
		struct list_head *node;
		struct list_head *tmp;
		timer_node_t *timer;
		list_for_each_safe(node, tmp, &list->head) {
			timer = list_entry(node, struct timer, node);
			list_del(node);
			free(timer->user);
			free(timer);
			timer = NULL;
		}
	}
        free(list);
        return 0;
 }

 int  add_timer(int interval, callback cb, void *user_data, int len)
 {
	struct timer *node = NULL;

	if (cb == NULL || interval <= 0) {
		return -1;
	}

	if(timer_list->num < timer_list->size) {
		timer_list->num++;
	} else {
		return -1;
	}

	if((node = malloc(sizeof(struct timer))) == NULL) {
		return -1;
	}
	if(user_data != NULL || len != 0) {
		node->user = malloc(len);
		memcpy(node->user, user_data, len);
		node->len = len;
	}

	node->cb = cb;
	node->interval = interval;
	node->elapse = 0;
	node->id =  timer_list->num;

	list_add_tail(&node->node, &timer_list->head);

	return node->id;
}

 int  del_timer(int timer_id)
 {
	struct list_head *node;
	struct list_head *tmp;
	timer_node_t *timer;

	if(timer_list == NULL) {
		return -1;
	}

	if(list_empty(&timer_list->head) == true){
		return -1;
	}

	list_for_each_safe(node, tmp, &timer_list->head) {
		timer = list_entry(node, struct timer, node);
		if (timer->id == timer_id){
			list_del(node);
			free(timer->user);
			free(timer);
			return 0;
		}
		timer = NULL;
	}
	return -1;
}

