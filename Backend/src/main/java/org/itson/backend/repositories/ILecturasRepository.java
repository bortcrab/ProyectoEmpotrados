package org.itson.backend.repositories;

import org.bson.types.ObjectId;
import org.itson.backend.collections.Lectura;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface ILecturasRepository extends MongoRepository<Lectura, ObjectId> {
}
