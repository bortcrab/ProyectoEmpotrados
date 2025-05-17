package org.itson.backend.collections;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.bson.types.ObjectId;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;

import java.util.Date;

/**
 * Clase colección que representa una lectura de un sensor.
 */
@Document(collection = "lecturas")
@Data
@AllArgsConstructor
@NoArgsConstructor
public class Lectura {
    @Id
    private ObjectId _id;
    private String tipo;
    private float valor;

    public Lectura(String tipo, float valor) {
        this.tipo = tipo;
        this.valor = valor;
    }
}
