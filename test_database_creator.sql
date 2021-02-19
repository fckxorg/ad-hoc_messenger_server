create schema messenger; 

create table messenger.Users (
	email text,
	handle varchar(256),
	public_key text
);

create table messenger.Messages (
	sender text,
	reciever text,
	payload text,
	datetime timestamp
);

insert into messenger.Users (email, handle, public_key)
values('max.kokryashkin@gmail.com', '@fckxorg', 'key');

select * from messenger.Users;
