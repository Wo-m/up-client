### TODO
## startup sequence
- fetch new transactions
- auto add transactions (see if entry exists where we expect it, if not add)
    - pay
    - rent
## important
- when fetching transactions do it one month backdated fetch
    - will have to redo write() to insert records in order, not just at the end
    - this will remove any card checks or cancelled transactions that we have already stored
    - at this point might be worth getting a lightweight DB
- convert all floats to ints where $1 = 100

## other
- store data more effeciently (i.e enum ints vs string representation)
