package org.itson.backend.controllers;

import org.itson.backend.dtos.LecturaDTO;
import org.itson.backend.services.LecturasService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Controller
@RequestMapping("/lecturas")
public class LecturasController {
    @Autowired
    private LecturasService service;

    /**
     * Método que maneja las peticiones HTTP POST.
     *
     * @param lectura Lectura a registrar.
     * @return La respuesta HTTP la lectura agregada y el código 200. Se devuelve un error si no se puede registrar.
     */
    @PostMapping("/registrar")
    @ResponseStatus(HttpStatus.CREATED)
    public ResponseEntity<?> registrarLectura(@RequestBody LecturaDTO lectura) {
        try {
            lectura = service.registrarLectura(lectura); // Se registra la lectura y se obtiene con su ID.
            ResponseEntity<LecturaDTO> respuesta = new ResponseEntity<>(lectura, HttpStatus.OK); // Se crea la respuesta.
            return respuesta;
        } catch (Exception e) {
            Map<String, String> error = new HashMap<>();
            error.put("mensaje", e.getMessage()); // Se mapea el error del mensaje.
            ResponseEntity<Map<String, String>> respuesta = new ResponseEntity<>(error, HttpStatus.NOT_FOUND); // Creamos la respuesta.
            return respuesta;
        }
    }

    /**
     * Método que maneja las peticiones GET.
     *
     * @return La respuesta HTTP con las lecturas encontrados y el status 200. Se devuelve un error si no se encontró nada.
     */
    @GetMapping({"", "/"})
    public ResponseEntity<?> obtenerTodasLecturas() {
        try {
            List<LecturaDTO> lecturas = service.obtenerTodasLecturas(); // Se obtienen las lecturas.
            ResponseEntity<List<LecturaDTO>> respuesta = new ResponseEntity<>(lecturas, HttpStatus.OK); // Se crea la respuesta.
            return respuesta;
        } catch (Exception e) {
            Map<String, String> error = new HashMap<>();
            error.put("mensaje", e.getMessage()); // Se mapea el error del mensaje.
            ResponseEntity<Map<String, String>> respuesta = new ResponseEntity<>(error, HttpStatus.NOT_FOUND); // Creamos la respuesta.
            return respuesta;
        }
    }
}
