#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


typedef void (*FnPtr) (void);

static pthread_mutex_t mut;
static pthread_cond_t cnd;
static FnPtr d;

static void*
native_starter (void *arg);

typedef void *MonoDomain;

typedef void *MonoObject;
typedef void *MonoAssembly;
typedef void *MonoImage;
typedef void *MonoClass;
typedef void *MonoMethod;
typedef void *MonoThread;

typedef int32_t mono_bool;

/** TODO: dlsym all this stuff from the current process.  Try to use runtime invoke from native to call the runtime. */

static MonoDomain *(*mono_get_root_domain) (void);

static MonoDomain *(*mono_domain_get)(void);

static mono_bool (*mono_domain_set)(MonoDomain *, mono_bool /*force */);

static MonoAssembly *(*mono_domain_assembly_open) (MonoDomain *, const char*);

static MonoImage *(*mono_assembly_get_image) (MonoAssembly *);

static MonoClass *(*mono_class_from_name)(MonoImage *, const char *, const char *);

static MonoMethod *(*mono_class_get_method_from_name)(MonoClass *, const char *, int /* arg_count */);

static MonoThread *(*mono_thread_attach)(MonoDomain *);

static void (*mono_thread_detach)(MonoThread *);

static MonoObject *(*mono_runtime_invoke) (MonoMethod *, void*, void**, MonoObject**);

#define GET_SYM(mod,name)					\
	do {							\
		name = dlsym (mod, #name);			\
		if (!name) {					\
			printf ("couldn't dlsym %s\n", #name);	\
			exit (1);				\
		}						\
	} while (0)

static void
setup_mono (void)
{
	void *mod = dlopen (NULL, 0);
	if (!mod) {
		printf ("couldn't dlopen\n");
		exit (1);
	}
	GET_SYM (mod, mono_get_root_domain);
	GET_SYM (mod, mono_domain_get);
	GET_SYM (mod, mono_domain_set);
	GET_SYM (mod, mono_domain_assembly_open);
	GET_SYM (mod, mono_assembly_get_image);
	GET_SYM (mod, mono_class_from_name);
	GET_SYM (mod, mono_class_get_method_from_name);
	GET_SYM (mod, mono_thread_attach);
	GET_SYM (mod, mono_thread_detach);
	GET_SYM (mod, mono_runtime_invoke);
}


static void do_call (void);


void FooStart (FnPtr p) {
	setup_mono();

	pthread_mutex_init (&mut, (void*)0);
	pthread_cond_init (&cnd, (void*)0);

	d = p;

	printf ("FooStart: starting native thread\n");
	pthread_mutex_lock (&mut);
	pthread_t t;
	pthread_create (&t, (void*)0, native_starter, (void*)0);
	pthread_detach (t);
	pthread_cond_wait (&cnd, &mut);
	pthread_mutex_unlock (&mut);
	printf ("FooStart: native thread signaled us, returning to managed\n");

	pthread_cond_destroy (&cnd);
	pthread_mutex_destroy (&mut);
}

static void*
native_starter (void *arg)
{
	pthread_mutex_lock (&mut);
	printf ("native thread: calling managed\n");
	do_call ();
	printf ("native thread: returned from managed\n");
	pthread_cond_signal (&cnd);
	pthread_mutex_unlock (&mut);
	//for (int i = 0; i < 2; ++i) {
	while (1) {
		printf ("native thread: sleeping\n");
		int didsleep = sleep (5);
		if (!didsleep) {
			printf ("sleep time elapsed\n");
		} else {
			printf ("woken up with %d seconds left\n", didsleep);
		}
#if 0
		printf ("calling mono again\n");
	        do_call ();
		printf ("returned from calling mono again\n");
#endif
	}
	return (void*)0;
}

static void do_call (void)
{
#if 0
	d ();
#else
	MonoDomain *domain = mono_domain_get ();
	MonoThread *thread = (void*)0;
	if (!domain) {
		thread = mono_thread_attach (mono_get_root_domain ());
	}
	domain = mono_domain_get ();
	if (!domain) {
		printf ("expected non-null domain after attaching\n");
		exit (1);
	}
	MonoAssembly *assm = mono_domain_assembly_open (domain, "Prog.exe");
	if (!assm) {
		printf ("expected non-null assembly\n");
		exit (1);
	}
	MonoImage *image = mono_assembly_get_image (assm);
	MonoClass *klass = mono_class_from_name (image, "", "Prog");
	if (!klass) {
		printf ("expected non-null class\n");
		exit (1);
	}
	MonoMethod *method = mono_class_get_method_from_name (klass, "SomeOtherMethod", 0);
	if (!method) {
		printf ("expected non-null method\n");
		exit (1);
	}
	MonoObject *args[1] = { NULL, };
	
	mono_runtime_invoke (method, (void*)0, (void**) args, NULL);

	if (thread)
		mono_thread_detach (thread);

#endif
}
