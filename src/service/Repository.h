#pragma once
#include "model/Tags.h"
#include "model/Transaction.h"
#include <sqlite_orm/sqlite_orm.h>

class Repository
{
public:
    static auto get_storage()
    {
        static auto storage = sqlite_orm::make_storage(
            "./info/up-client.db",
            sqlite_orm::make_table(
                "Transaction",
                sqlite_orm::make_column("id", &Transaction::id, sqlite_orm::primary_key().autoincrement()),
                sqlite_orm::make_column("amount", &Transaction::amount),
                sqlite_orm::make_column("description", &Transaction::description),
                sqlite_orm::make_column("created_at", &Transaction::createdAt),
                sqlite_orm::make_column("tag", &Transaction::tag),
                sqlite_orm::make_column("manual", &Transaction::manual)));
        return storage;
    }
};
