/*
 * Copyright (c) 2003, 2004 TADA AB - Taby Sweden
 * Distributed under the terms shown in the file COPYRIGHT.
 * 
 * @author Thomas Hallgren
 */
#include <postgres.h>
#include <executor/spi.h>
#include <executor/tuptable.h>

#include "org_postgresql_pljava_internal_TupleTableSlot.h"
#include "pljava/Exception.h"
#include "pljava/type/Type_priv.h"
#include "pljava/type/TupleTableSlot.h"
#include "pljava/type/Tuple.h"
#include "pljava/type/TupleDesc.h"

static Type      s_TupleTableSlot;
static TypeClass s_TupleTableSlotClass;
static jclass    s_TupleTableSlot_class;
static jmethodID s_TupleTableSlot_init;

/*
 * org.postgresql.pljava.type.Tuple type.
 */
jobject TupleTableSlot_create(JNIEnv* env, TupleTableSlot* tts)
{
	jobject jtts;
	if(tts == 0)
		return 0;

	jtts = MemoryContext_lookupNative(env, tts);
	if(jtts == 0)
	{
		jtts = PgObject_newJavaObject(env, s_TupleTableSlot_class, s_TupleTableSlot_init);
		NativeStruct_init(env, jtts, tts);
	}
	return jtts;
}

static jvalue _TupleTableSlot_coerceDatum(Type self, JNIEnv* env, Datum arg)
{
	jvalue result;
	result.l = TupleTableSlot_create(env, (TupleTableSlot*)DatumGetPointer(arg));
	return result;
}

static Type TupleTableSlot_obtain(Oid typeId)
{
	return s_TupleTableSlot;
}

/* Make this datatype available to the postgres system.
 */
extern Datum TupleTableSlot_initialize(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(TupleTableSlot_initialize);
Datum TupleTableSlot_initialize(PG_FUNCTION_ARGS)
{
	JNINativeMethod methods[] = {
		{
		"_getTuple",
	  	"()Lorg/postgresql/pljava/internal/Tuple;",
	  	Java_org_postgresql_pljava_internal_TupleTableSlot__1getTuple
		},
		{
		"_getTupleDesc",
		"()Lorg/postgresql/pljava/internal/TupleDesc;",
		Java_org_postgresql_pljava_internal_TupleTableSlot__1getTupleDesc
		},
		{ 0, 0, 0 }};

	JNIEnv* env = (JNIEnv*)PG_GETARG_POINTER(0);

	s_TupleTableSlot_class = (*env)->NewGlobalRef(
				env, PgObject_getJavaClass(env, "org/postgresql/pljava/internal/TupleTableSlot"));

	PgObject_registerNatives2(env, s_TupleTableSlot_class, methods);

	s_TupleTableSlot_init = PgObject_getJavaMethod(
				env, s_TupleTableSlot_class, "<init>", "()V");

	s_TupleTableSlotClass = NativeStructClass_alloc("type.TupleTableSlot");
	s_TupleTableSlotClass->JNISignature   = "Lorg/postgresql/pljava/internal/TupleTableSlot;";
	s_TupleTableSlotClass->javaTypeName   = "org.postgresql.pljava.internal.TupleTableSlot";
	s_TupleTableSlotClass->coerceDatum    = _TupleTableSlot_coerceDatum;
	s_TupleTableSlot = TypeClass_allocInstance(s_TupleTableSlotClass, InvalidOid);

	Type_registerJavaType("org.postgresql.pljava.internal.TupleTableSlot", TupleTableSlot_obtain);
	PG_RETURN_VOID();
}

/****************************************
 * JNI methods
 ****************************************/
/*
 * Class:     org_postgresql_pljava_internal_TupleTableSlot
 * Method:    _getTuple
 * Signature: ()Lorg/postgresql/pljava/internal/Tuple;
 */
JNIEXPORT jobject JNICALL
Java_org_postgresql_pljava_internal_TupleTableSlot__1getTuple(JNIEnv* env, jobject _this)
{
	TupleTableSlot* slot;
	PLJAVA_ENTRY_FENCE(0)
	slot = (TupleTableSlot*)NativeStruct_getStruct(env, _this);
	if(slot == 0)
		return 0;
	return Tuple_create(env, slot->val);
}

/*
 * Class:     org_postgresql_pljava_internal_TupleTableSlot
 * Method:    _getTupleDesc
 * Signature: ()Lorg/postgresql/pljava/internal/TupleDesc;
 */
JNIEXPORT jobject JNICALL
Java_org_postgresql_pljava_internal_TupleTableSlot__1getTupleDesc(JNIEnv* env, jobject _this)
{
	TupleTableSlot* slot;
	PLJAVA_ENTRY_FENCE(0)
	slot = (TupleTableSlot*)NativeStruct_getStruct(env, _this);
	if(slot == 0)
		return 0;
	return TupleDesc_create(env, slot->ttc_tupleDescriptor);
}
