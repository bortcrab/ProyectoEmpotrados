package org.itson.backend.services;

import org.itson.backend.collections.Lectura;
import org.itson.backend.dtos.LecturaDTO;
import org.itson.backend.repositories.ILecturasRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

@Service
public class LecturasService {
    @Autowired
    private ILecturasRepository repository;
    SimpleDateFormat formato = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");

    /**
     * Método para registrar una lectura.
     *
     * @param lecturaDTO La lectura a registrar.
     * @return La lectura que se registró.
     * @throws Exception En caso de que ocurra un error durante el registro.
     */
    public LecturaDTO registrarLectura(LecturaDTO lecturaDTO) throws Exception {
        Lectura lectura = convertirLectura(lecturaDTO);
        lectura.setFecha(new java.util.Date());
        repository.save(lectura);
        return lecturaDTO;
    }

    /**
     * Método que obtiene todas las lecturas.
     *
     * @return Una lista con todas las lecturas encontradas.
     * @throws Exception en caso de que no se haya encontrado ninguna lectura.
     */
    public List<LecturaDTO> obtenerTodasLecturas() throws Exception {
        List<Lectura> lecturasObtenidas = repository.findAll(); // Se obtienen los lecturas.
        if (!lecturasObtenidas.isEmpty()) {
            List<LecturaDTO> lecturas = convertirLecturasDTO(lecturasObtenidas);
            return lecturas; // Se devuelven.
        } else {
            throw new Exception("No se encontró ninguna lectura.");
        }
    }

    private List<LecturaDTO> convertirLecturasDTO(List<Lectura> lecturasObtenidas) {
        List <LecturaDTO> lecturasDTO = new ArrayList<>();
        for (Lectura lectura : lecturasObtenidas) {
            LecturaDTO lecturaDTO = convertirLecturaDTO(lectura);
            lecturasDTO.add(lecturaDTO);
        }
        return lecturasDTO;
    }

    private LecturaDTO convertirLecturaDTO(Lectura lectura) {
        return new LecturaDTO(
                lectura.getTipo(),
                lectura.getValor(),
                formatearFecha(lectura.getFecha())
        );
    }

    private String formatearFecha(Date fecha) {
        return formato.format(fecha);
    }

    private Lectura convertirLectura(LecturaDTO lecturaDTO) {
        return new Lectura(
                lecturaDTO.getTipo(),
                lecturaDTO.getValor()
        );
    }
}
